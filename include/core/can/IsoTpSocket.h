//
// Created by tyll on 08.09.22.
//

#ifndef UC_CORE_ISOTPSOCKET_H
#define UC_CORE_ISOTPSOCKET_H

#include "ICanInterface.h"
#include "core/Tick.h"
#include "iso15765-canbus/src/lib_iso15765.h"
#include "etl/queue_spsc_atomic.h"
#include "core/shared_ptr.h"
#include "core/coop/Future.h"

#ifndef UC_CORE_ISOTP_QUEUE_SIZE
#define UC_CORE_ISOTP_QUEUE_SIZE 8
#endif

namespace core { namespace can {
    class IsoTpPacket {
    public:
        IsoTpPacket() : _length(0), _data(core::unique_ptr<uint8_t[]>()) {}

        explicit IsoTpPacket(canid_t canid, size_t length) : _canid(canid), _length(length), _data(core::unique_ptr<uint8_t[]>(new uint8_t[length])) {

        }

        inline canid_t canid() { return _canid; }

        inline size_t length() { return _length; }

        inline uint8_t* getData() {
            return _data.get();
        }

    private:
        canid_t _canid;
        size_t _length;
        core::unique_ptr<uint8_t[]> _data;
    };

    class IsoTpSocket {
    public:

        IsoTpSocket(core::coop::IDispatcher& dispatcher, ICanInterface& canInterface, canid_t txId, canid_t rxId);
        IsoTpSocket(IsoTpSocket& socket) = delete;
        IsoTpSocket(IsoTpSocket&& socket);
        ~IsoTpSocket();
        void send(uint8_t* data, uint16_t length);
        bool available() const;
        bool tryReceive(IsoTpPacket& outPacket) const;
        void receiveAndTransmit();
    private:
        iso15765_t createIsoTpHandler() const;
        void startBackgroundWorker(core::coop::IDispatcher &dispatcher);


    private:
        ICanInterface& _can;
        iso15765_t _isotp;
        shared_ptr<coop::IFuture> _backgroundWorkerTask;
    };
}}
namespace core { namespace can {
static inline core::can::ICanInterface* _core_can_isotp_glue_iface = nullptr;
static inline core::can::IsoTpSocket* _core_can_isotp_glue_socket = nullptr;
static inline canid_t _core_can_isotp_glue_txid = 0;
static inline canid_t _core_can_isotp_glue_rxid = 0;
static inline etl::queue_spsc_atomic<core::can::IsoTpPacket, UC_CORE_ISOTP_QUEUE_SIZE> _core_can_isotp_glue_received = etl::queue_spsc_atomic<core::can::IsoTpPacket, UC_CORE_ISOTP_QUEUE_SIZE>();

static inline void core_can_isotp_glue_init(core::can::IsoTpSocket* socket, core::can::ICanInterface* iface, canid_t txid, canid_t rxid) {
    if (_core_can_isotp_glue_socket == nullptr && _core_can_isotp_glue_iface == nullptr) {
        _core_can_isotp_glue_socket = socket;
        _core_can_isotp_glue_iface = iface;
        _core_can_isotp_glue_txid = txid;
        _core_can_isotp_glue_rxid = rxid;
        _core_can_isotp_glue_received.clear();
    }
}
static inline void core_can_isotp_glue_deinit(core::can::IsoTpSocket* socket) {
    if (_core_can_isotp_glue_socket == socket) {
        _core_can_isotp_glue_socket = nullptr;
        _core_can_isotp_glue_iface = nullptr;
        _core_can_isotp_glue_received.clear();
    }
}

extern "C" {
    static inline uint8_t core_can_isotp_glue_iface_send_frame(cbus_id_type id_type, uint32_t id, cbus_fr_format fr_fmt, uint8_t dlc, uint8_t* dt) {
        if (_core_can_isotp_glue_iface == nullptr)
            return 0;
        core::can::CanFrame f = {};
        f.id = _core_can_isotp_glue_txid;
        f.length = 8;
        memset(f.payload, 0x55, sizeof(f.payload));
        memcpy(f.payload, dt, dlc);
        _core_can_isotp_glue_iface->writeFrame(f);
        return 0;
    }

    static inline void core_can_isotp_glue_iface_usdata_indication(n_indn_t* info) {
        canid_t id = 0x80
                          | info->n_ai.n_pr << 8
                          | info->n_ai.n_ta << 3
                          | info->n_ai.n_sa
                          | (info->n_ai.n_tt == N_TA_T_PHY ? 0x40U : 0x00U);
        core::can::IsoTpPacket p(id, info->msg_sz);
        memcpy(p.getData(), info->msg, p.length());
        _core_can_isotp_glue_received.push(etl::move(p));
    }

    static  inline uint32_t core_can_isotp_glue_getms() {
        return static_cast<uint32_t>(millis());
    }

    static inline void core_can_isotp_glue_on_error(n_rslt err_type) {
        // ignore for now...
    }
}

    IsoTpSocket::IsoTpSocket(core::coop::IDispatcher& dispatcher, ICanInterface& canInterface, canid_t txId, canid_t rxId) : _can(canInterface) {
        iso15765_t handler = createIsoTpHandler();
        _isotp = handler;
        core_can_isotp_glue_init(this, &_can, txId, rxId);
        iso15765_init(&_isotp);
        startBackgroundWorker(dispatcher);
    }

    iso15765_t IsoTpSocket::createIsoTpHandler() const {
        iso15765_t handler = {};
        handler.fr_addr_md = N_ADM_NORMAL;
        handler.fr_id_type = CBUS_ID_T_STANDARD;
        handler.config.stmin = 0x05;
        handler.config.bs = 0x0F;
        handler.config.n_bs = 800;
        handler.config.n_cr = 250;
        handler.clbs.get_ms = core_can_isotp_glue_getms;
        handler.clbs.send_frame = core_can_isotp_glue_iface_send_frame;
        handler.clbs.on_error = core_can_isotp_glue_on_error;
        handler.clbs.indn = core_can_isotp_glue_iface_usdata_indication;
        return handler;
    }

    IsoTpSocket::IsoTpSocket(IsoTpSocket&& socket) : _can(socket._can), _isotp(socket._isotp) {

    }

    IsoTpSocket::~IsoTpSocket() {
        _backgroundWorkerTask->cancel();
        core_can_isotp_glue_deinit(this);
    }

    void IsoTpSocket::send(uint8_t *data, uint16_t length) {
        canid_t id = _core_can_isotp_glue_txid;
        n_req_t f = {};
        f.n_ai.n_pr = (uint8_t)((id & 0x700U) >> 8);
        f.n_ai.n_ta = (uint8_t)((id & 0x38U) >> 3);
        f.n_ai.n_sa = (uint8_t)(id & 0x07U);
        f.n_ai.n_tt = (((id & 0x40U) >> 6) == 1 ? N_TA_T_PHY : N_TA_T_FUNC);
        f.fr_fmt = CBUS_FR_FRM_STD;
        memcpy(f.msg, data, length);
        f.msg_sz = length;
        if (length < I15765_MSG_SIZE)
            memset(f.msg + length, 0x55, I15765_MSG_SIZE - length);
        iso15765_send(&_isotp, &f);
    }

    bool IsoTpSocket::available() const {
        return _core_can_isotp_glue_received.size() > 0;
    }

    bool IsoTpSocket::tryReceive(IsoTpPacket& outPacket) const {
        if (!available())
            return false;
        _core_can_isotp_glue_received.pop(outPacket);
        return true;
    }

    void IsoTpSocket::receiveAndTransmit()  {
        CanFrame f;
        if (_can.tryReadFrame(f) /* && f.id == _core_can_isotp_glue_rxid */ ) {
            canbus_frame_t cf;
            cf.id_type = CBUS_ID_T_STANDARD;
            cf.fr_format = CBUS_FR_FRM_STD;
            cf.id = f.id;
            cf.dlc = f.length;
            memcpy(cf.dt, f.payload, f.length);
            iso15765_enqueue(&_isotp, &cf);
        }
        iso15765_process(&_isotp);
    }

    void IsoTpSocket::startBackgroundWorker(coop::IDispatcher &dispatcher) {
        _backgroundWorkerTask = core::coop::async([self = this](){
            self->receiveAndTransmit();
            return coop::yieldContinue();
        }).share();

        dispatcher.run(_backgroundWorkerTask);
    }
}}

#endif //UC_CORE_ISOTPSOCKET_H

//
// Created by tyll on 06.02.22.
//

#ifndef FIRMWARE_EVENTUAL_H
#define FIRMWARE_EVENTUAL_H
namespace core { namespace async {
    // TODO: replace usages (ObdTransceiver) with Future.

    template<typename TData>
    class Promise {
    public:
        TData& get() {
            return _value;
        }
        bool isAvailable() {
            return _isAvailable;
        }
        void set(TData value){
            if (_isAvailable)
                return;

            _value = value;
            _isAvailable = true;
        }
        void reset() {
            _isAvailable = false;
            _value = TData();
        }
    private:
        TData _value;
        volatile bool _isAvailable = true;
    };
}}
#endif //FIRMWARE_EVENTUAL_H

//
// Created by tyll on 2022-01-28.
//

#ifndef SGLOGGER_EVENTHANDLER_H
#define SGLOGGER_EVENTHANDLER_H

template <typename TSender, typename TEventArgs>
class IEventHandler
{
public:

    typedef TEventArgs parameter_type; ///< The type of parameter sent to the EventHandler.

    virtual ~IEventHandler() {}

    //*************************************************************************
    /// The EventHandler operator that will be overridden.
    //*************************************************************************
    virtual void operator ()(TSender, TEventArgs) const = 0;
};

//***************************************************************************
///\ingroup EventHandler
/// The base interface template for functions taking <b>void</b> parameters.
//***************************************************************************
template <typename TSender>
class IEventHandler<TSender, void>
{
public:

    typedef void parameter_type; ///< The type of parameter sent to the EventHandler.

    virtual ~IEventHandler() {}

    //*************************************************************************
    /// The EventHandler operator that will be overridden.
    //*************************************************************************
    virtual void operator ()() const = 0;
};

//***************************************************************************
///\ingroup EventHandler
/// A derived EventHandler template that takes an object type and parameter type.
///\tparam TListener    The object type that contains the member EventHandler.
///\tparam TEventArgs The parameter type accepted by the member EventHandler.
//***************************************************************************
template <typename TSender, typename TListener, typename TEventArgs>
class EventHandler : public IEventHandler<TSender, TEventArgs>
{
public:

    typedef TListener    object_type;    ///< The type of object.
    typedef TEventArgs parameter_type; ///< The type of parameter sent to the EventHandler.

    //*************************************************************************
    /// Constructor.
    ///\param object    Reference to the object
    ///\param p_function Pointer to the member EventHandler
    //*************************************************************************
    EventHandler(TListener& object_, void(TListener::* p_function_)(TSender, TEventArgs))
            : p_object(&object_),
              p_function(p_function_)
    {
    }

    //*************************************************************************
    /// The EventHandler operator that calls the destination EventHandler.
    ///\param data The data to pass to the EventHandler.
    //*************************************************************************
    virtual void operator ()(TSender sender, TEventArgs data) const override
    {
        // Call the object's member EventHandler with the data.
        (p_object->*p_function)(data);
    }

private:

    TListener* p_object;                        ///< Pointer to the object that contains the EventHandler.
    void (TListener::* p_function)(TSender, TEventArgs); ///< Pointer to the member EventHandler.
};

//***************************************************************************
///\ingroup EventHandler
/// A derived EventHandler template that takes a parameter type.
///\tparam TListener The object type that contains the member EventHandler.
//***************************************************************************
template <typename TSender, typename TListener>
class EventHandler<TSender, TListener, void> : public IEventHandler<TSender, void>
{
public:

    //*************************************************************************
    /// Constructor.
    ///\param object   Reference to the object
    ///\param p_function Pointer to the member EventHandler
    //*************************************************************************
    EventHandler(TListener& object_, void(TListener::* p_function_)(TSender))
            : p_object(&object_),
              p_function(p_function_)
    {
    }

    //*************************************************************************
    /// The EventHandler operator that calls the destination EventHandler.
    //*************************************************************************
    virtual void operator ()(TSender sender) const override
    {
        // Call the object's member EventHandler.
        (p_object->*p_function)(sender);
    }

private:

    TListener* p_object;              ///< Pointer to the object that contains the EventHandler.
    void (TListener::* p_function)(TSender); ///< Pointer to the member EventHandler.
};

//***************************************************************************
///\ingroup EventHandler
/// Specialisation for static or global functions that takes a parameter.
//***************************************************************************
template <typename TSender, typename TEventArgs>
class EventHandler<TSender, void, TEventArgs> : public IEventHandler<TSender, TEventArgs>
{
public:

    //*************************************************************************
    /// Constructor.
    ///\param p_function Pointer to the EventHandler
    //*************************************************************************
    explicit EventHandler(void(*p_function_)(TEventArgs))
            : p_function(p_function_)
    {
    }

    //*************************************************************************
    /// The EventHandler operator that calls the destination EventHandler.
    ///\param data The data to pass to the EventHandler.
    //*************************************************************************
    virtual void operator ()(TEventArgs data) const override
    {
        // Call the EventHandler with the data.
        (*p_function)(data);
    }

private:

    void (*p_function)(TEventArgs); ///< Pointer to the EventHandler.
};

//***************************************************************************
///\ingroup EventHandler
/// Specialisation static functions taking void parameter.
//***************************************************************************
template <typename TSender>
class EventHandler<TSender, void, void> : public IEventHandler<TSender, void>
{
public:

    //*************************************************************************
    /// Constructor.
    ///\param p_function Pointer to the EventHandler.
    //*************************************************************************
    explicit EventHandler(void(*p_function_)(TSender))
            : p_function(p_function_)
    {
    }

    //*************************************************************************
    /// The EventHandler operator that calls the destination EventHandler.
    //*************************************************************************
    virtual void operator ()(TSender sender) const override
    {
        // Call the EventHandler.
        (*p_function)(sender);
    }

private:

    void (*p_function)(TSender); ///< Pointer to the EventHandler.
};

#endif //SGLOGGER_EVENTHANDLER_H

#pragma once

#include "Translators/sfBaseTranslator.h"

#include <CoreMinimal.h>
#include <Queue.h>
#include <sfObject.h>
#include <sfDictionaryProperty.h>
#include <sfListProperty.h>
#include <ksEvent.h>
#include <unordered_map>
#include <unordered_set>

using namespace KS;

/**
 * The object event dispatcher listens for object events and calls the corresponding functions on the translator
 * registered for the object's type.
 */
class sfObjectEventDispatcher
{
public:
    typedef std::shared_ptr<sfObjectEventDispatcher> SPtr;

    /**
     * Static shared pointer constructor.
     *
     * @return  SPtr
     */
    static SPtr CreateSPtr();

    /**
     * Constructor
     */
    sfObjectEventDispatcher();

    /**
     * Destructor
     */
    ~sfObjectEventDispatcher();

    /**
     * Registers a translator to handle events for a given object type.
     *
     * @param   const sfName& objectType the translator should handle events for.
     * @param   TSharedPtr<sfBaseTranslator> translatorPtr to register.
     */
    void Register(const sfName& objectType, TSharedPtr<sfBaseTranslator> translatorPtr);

    /**
     * Starts listening for events and calls Initialize on all registered translators.
     */
    void Initialize();

    /**
     * Stops listening for events and calls CleanUp on all registered translators.
     */
    void CleanUp();

    /**
     * Creates an sfObject for a uobject by calling Create on each translator until one of them handles the request.
     *
     * @param   uobjPtr to create sfObject for.
     * @return  sfObject::SPtr for the uobject. May be nullptr.
     */
    sfObject::SPtr Create(UObject* uobjPtr);

    /**
     * Checks if an object is queued to be created locally.
     *
     * @param   sfObject::SPtr objPtr to check.
     */
    bool IsCreateQueued(sfObject::SPtr objPtr);

    /**
     * Queues an object to be created locally.
     *
     * @param   sfObject::SPtr objPtr to queue.
     */
    void QueueCreate(sfObject::SPtr objPtr);

    /**
     * Creates objects from the create queue.
     */
    void ProcessCreateQueue();

    /**
     * Calls OnCreate on the translator for an object.
     *
     * @param   sfObject::SPtr objPtr that was created.
     * @param   int childIndex the object was created at.
     */
    void OnCreate(sfObject::SPtr objPtr, int childIndex);

    /**
     * Calls OnPropertyChange on the translator for an object.
     *
     * @param   sfProperty::SPtr propPtr that changed.
     */
    void OnPropertyChange(sfProperty::SPtr propPtr);

    /**
     * Calls OnUPropertyChange on the translator for an object.
     *
     * @param   sfObject::SPtr objPtr for the uobject whose property changed.
     * @param   UObject* uobjPtr whose property changed.
     * @param   UProperty* upropPtr that changed.
     */
    bool OnUPropertyChange(sfObject::SPtr objPtr, UObject* uobjPtr, UProperty* upropPtr);

    /**
     * Calls PostPropertyChange on the translator for an object.
     *
     * @param   UObject* uobjPtr whose property changed.
     * @param   UProperty* upropPtr that changed.
     */
    void PostPropertyChange(UObject* uobjPtr, UProperty* upropPtr);

    /**
     * Calls OnUndoRedo on the translator for an object.
     *
     * @param   sfObject::SPtr objPtr for the uobject changed by the transaction. nullptr if the uobject is not synced.
     * @param   UObject* uobjPtr changed by the transcation.
     */
    void OnUndoRedo(sfObject::SPtr objPtr, UObject* uobjPtr);

    /**
     * Enables OnUObjectModified events.
     */
    void EnableOnUObjectModified();

    /**
     * Disables OnUObjectModified events.
     */
    void DisableOnUObjectModified();

    /**
     * Gets the object translator for an object.
     *
     * @param   sfObject::SPtr objPtr to get translator for.
     * @return  TSharedPtr<sfBaseTranslator> translator for the object,
     *          or nullptr if there is no translator for the object's type.
     */
    TSharedPtr<sfBaseTranslator> GetTranslator(sfObject::SPtr objPtr);

    /**
     * Gets the object translator for the given type.
     *
     * @param   const sfName& type
     * @return  TSharedPtr<sfBaseTranslator> translator for the type,
     *          or nullptr if there is no translator for the given type.
     */
    TSharedPtr<sfBaseTranslator> GetTranslator(const sfName& type);

private:
    bool m_active;
    std::unordered_map<sfName, TSharedPtr<sfBaseTranslator>> m_translatorMap;
    // Some translators are registered in the map more than once, so we also have a set of translators for iteration.
    TSet<TSharedPtr<sfBaseTranslator>> m_translators;
    TQueue<sfObject::SPtr> m_createQueue;
    std::unordered_set<sfObject::SPtr> m_createSet;
    ksEvent<sfObject::SPtr&, int&>::SPtr m_createEventPtr;
    ksEvent<sfObject::SPtr&>::SPtr m_deleteEventPtr;
    ksEvent<sfObject::SPtr&>::SPtr m_lockEventPtr;
    ksEvent<sfObject::SPtr&>::SPtr m_unlockEventPtr;
    ksEvent<sfObject::SPtr&>::SPtr m_lockOwnerChangeEventPtr;
    ksEvent<sfObject::SPtr&>::SPtr m_directLockChangeEventPtr;
    ksEvent<sfObject::SPtr&, int&>::SPtr m_parentChangeEventPtr;
    ksEvent<sfProperty::SPtr&>::SPtr m_propertyChangeEventPtr;
    ksEvent<sfDictionaryProperty::SPtr&, sfName&>::SPtr m_removeFieldEventPtr;
    ksEvent<sfListProperty::SPtr&, int&, int&>::SPtr m_listAddEventPtr;
    ksEvent<sfListProperty::SPtr&, int&, int&>::SPtr m_listRemoveEventPtr;
    FDelegateHandle m_onObjectModifiedHandle;
};
namespace PopupManager
{
    //This interface is given to exterior systems and controllers that want to add a popup to the queue or find some basic information
    public interface IPopupManagerController
    {
        ILoadPopupOperationObserver QueueImmediatePopup(IPopupRequest request);
        ILoadPopupOperationObserver InsertImmediatePopupAtFrontOfQueue(IPopupRequest request);
        void RemoveFromQueue(PopupIdentifier identifier);
        void ClearQueue();
        int QueueCount();
        bool IsDisplayingPopup();
        bool IsLoadingPopup();
        PopupIdentifier GetCurrentPopupIdentifier();
        ILockHandle Lock();
        void Unlock(ILockHandle lockHandle);
        bool IsLocked();
    }
}
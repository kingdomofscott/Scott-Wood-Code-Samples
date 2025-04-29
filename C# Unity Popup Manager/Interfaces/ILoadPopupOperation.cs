namespace PopupManager
{
    //This interface is used to encapsulate loading functionality calls
    public interface IPopupLayerController
    {
        bool IsLoaded();
        bool IsLoading();
        OperationState GetState();
        OperationState Update();
        float GetProgress();
        void Abort();
        void Reset();
        Result GetResult();
    }
}
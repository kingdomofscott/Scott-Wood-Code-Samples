
namespace PopupManager
{
    public class LayerState : PopupLayerController.State, ICompleteController
    {
        public LayerState(PopupLayerController newParentStateMachine) : base(newParentStateMachine)
        {
        }

        public void CheckNextPopupOrClose()
        {
            QueuePopupOperation currentPopup = ParentStateMachine.CurrentPopupOperation;
            QueuePopupOperation nextPopup = null;

            if (ParentStateMachine.PopupManager.PopupLayerCount == 1)//Only check popup queue if this is the only layer
            {
                nextPopup = ParentStateMachine.PopupManager.CheckForNextPopup();
            }

            if (nextPopup == null)
            {
                if(currentPopup == ParentStateMachine.CurrentPopupOperation)
                {//Only close if a surface point did not take over in CheckForNextPopup
                    ParentStateMachine.CloseLayer();
                }

                return;
            }

            ParentStateMachine.SetupNewPopupOperation(nextPopup);
            ParentStateMachine.CurrentPopupOperation.Update();
        }

        public virtual void ManagedOnDestroy()
        {
        }

        public virtual void ManagedUpdate()
        {
        }
    }
}

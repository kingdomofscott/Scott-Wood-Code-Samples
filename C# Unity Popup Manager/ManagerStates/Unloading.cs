
namespace PopupManager.States
{
    public class Unloading : State
    {
        public Unloading(PopupManagerController newParentStateMachine) : base(newParentStateMachine)
        {
        }

        public override void ManagedUpdate()
        {
            if (ParentStateMachine.IsUnloadingUnusedAssets())
            {
                return;
            }

            if (ParentStateMachine.IsDisplayingPopup())
            {
                ParentStateMachine.SwitchToDisplaying();
            }
            else
            {
                nextPopup = ParentStateMachine.CheckForNextPopup();

            }
        }
    }
}
namespace PopupManager.States
{
    public class Idle : State
    {
        public Idle(PopupManagerController newParentStateMachine) : base(newParentStateMachine)
        {
        }

        public override void OnEnter()
        {
            ParentStateMachine.View.DisableBlocker();
        }

        public override void ManagedUpdate()//Constantly check if we need to start the process of showing a popup
        {
            QueuePopupOperation nextPopup = ParentStateMachine.CheckForNextPopup();

            if (nextPopup != null)
            {
                ParentStateMachine.AddPopupOnNewTopLayer(nextPopup, false);
            }
        }
    }
}

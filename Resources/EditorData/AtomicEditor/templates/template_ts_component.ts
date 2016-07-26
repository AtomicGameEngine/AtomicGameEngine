"atomic component";

/**
 * A new component
 */
export default class Component extends Atomic.JSComponent {

    /**
     * Fields witihin the inspectorFields object will be exposed to the editor
     */
    inspectorFields = {
    };

    /**
     * Called when the component is first added to the node
     */
    start() {

    }

    /**
     * Update called every cycle with timeStep containing the delta between calls
     * @param timeStep time since last call to update
     */
    update(timeStep: number) {

    }
}

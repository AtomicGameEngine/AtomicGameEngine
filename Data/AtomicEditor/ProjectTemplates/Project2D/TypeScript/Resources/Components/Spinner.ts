"atomic component";

/**
 * Component that will rotate a node at a configurable speed.
 */
export default class Star extends Atomic.JSComponent {

    /**
     * Fields within the inspectorFields object will be exposed to the editor
     */
    inspectorFields = {
        speed: 1.0
    };

    /**
     * The speed at which to turn the component
     * @type {number}
     */
    speed: number;

    /**
     * Update called every cycle with timeStep containing the delta between calls
     * @param  {number} timeStep time since last call to update in seconds
     */
    update(timeStep) {

        this.node.roll(this.speed * timeStep * 75);

    }
}

//Atomic TypeScript Definitions


/// <reference path="Atomic.d.ts" />

declare module AtomicPlayer {


//----------------------------------------------------
// MODULE: Player
//----------------------------------------------------


   export class Player extends Atomic.AObject {

      // Construct.
      constructor();

      loadScene(filename: string, camera?: Atomic.Camera): Atomic.Scene;
      sayHi(): void;

   }



}

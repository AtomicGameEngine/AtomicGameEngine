/**
 * Defines the interface to what is available for the host to call or for the client to call on the window object
 */
interface Window {
    atomicQuery: any;
    HOST_loadCode: (codeUrl) => void;
    HOST_saveCode: () => void;
}

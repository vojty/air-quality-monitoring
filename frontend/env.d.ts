/// <reference types="vite/client" />

interface ImportMetaEnv {
  // only VITE_ prefixed end variable are exposed to client, see https://vitejs.dev/guide/env-and-mode.html#env-files
  readonly VITE_WS_HOSTNAME: string | undefined;
}

interface ImportMeta {
  readonly env: ImportMetaEnv;
}

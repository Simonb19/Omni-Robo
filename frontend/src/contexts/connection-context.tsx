import { createContext } from "react";
import type { ConnectionStatus } from "../types";

const ConnectionContext = createContext<ConnectionStatus | undefined>(undefined);

export default ConnectionContext;
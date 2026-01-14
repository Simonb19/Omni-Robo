import { useContext } from "react";
import ConnectionContext from "../contexts/connection-context";

export function useConnection() {
  const ctx = useContext(ConnectionContext);
  if (!ctx) throw new Error("useConnection must be used within ConnectionContextProvider");
  return ctx;
}
import { ReactNode } from "react";
import { formatTimestamp } from "../formatters";

export function Station({
  children,
  title,
  lastUpdate,
}: {
  children: ReactNode;
  title: ReactNode;
  lastUpdate: number;
}) {
  return (
    <div
      style={{
        padding: "1rem 0.5rem",
        marginBottom: "1rem",
        borderRadius: 10,
        background: "#ddd",
      }}
    >
      <div style={{ fontSize: "1.5rem", fontWeight: 500, textAlign: "center" }}>
        {title}
      </div>
      <div
        style={{
          display: "flex",
          flexDirection: "column",
          gap: "1rem",
          margin: "1rem 0",
        }}
      >
        {children}
      </div>
      <small
        style={{
          justifyContent: "center",
          display: "flex",
          alignItems: "center",
        }}
      >
        <span style={{ marginLeft: "0.5rem", fontVariant: "tabular-nums" }}>
          Last update @ {formatTimestamp(lastUpdate)}
        </span>
      </small>
    </div>
  );
}

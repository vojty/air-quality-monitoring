import { ReactNode } from "react";
import { formatTimestamp } from "../formatters";

export function Station({
  children,
  header,
  interval,
  lastUpdate,
}: {
  children: ReactNode;
  header: ReactNode;
  interval: number;
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
      <div style={{ fontSize: "1.5rem", fontWeight: 500, textAlign: "left" }}>
        {header}
      </div>
      <div
        style={{
          display: "flex",
          flexDirection: "column",
          gap: "0.5rem",
          margin: "1rem 0",
        }}
      >
        {children}
      </div>
      <small
        style={{
          display: "flex",
          alignItems: "center",
          margin: "0 0.5rem",
        }}
      >
        <span style={{ fontVariant: "tabular-nums" }}>
          Last update @ {formatTimestamp(lastUpdate)}
        </span>

        {interval > 0 && (
          <div style={{ marginLeft: "auto" }}>Interval: {interval}s</div>
        )}
      </small>
    </div>
  );
}

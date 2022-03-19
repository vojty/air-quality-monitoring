import { ReactNode } from "react";

export function Box({
  icon,
  title,
  text,
}: {
  icon: ReactNode;
  title: ReactNode;
  text: ReactNode;
}) {
  return (
    <div
      style={{
        alignItems: "center",
        display: "flex",
        padding: "1.5rem 1rem",
        background: "#ffedd5",
        borderRadius: 10,
      }}
    >
      <div>
        <div style={{ fontSize: "1.85rem", margin: "0 0.5rem 0.25rem 0" }}>
          {icon}
        </div>
        <div style={{ fontSize: "0.9rem" }}>{title}</div>
      </div>
      <div
        style={{
          fontSize: "3.45rem",
          marginLeft: "auto",
          fontVariant: "tabular-nums",
        }}
      >
        {text}
      </div>
    </div>
  );
}

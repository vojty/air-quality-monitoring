import classes from "./Status.module.css";

type Props = {
  size?: number;
  color?: string;
  animate?: boolean;
};

export function Status(props: Props) {
  const { size = 10, color = "black", animate = false } = props;
  const commonProps = {
    height: size,
    width: size,
  };
  return (
    <div>
      {animate && (
        <div
          className={classes.ring}
          style={{ ...commonProps, borderColor: color }}
        />
      )}
      <div
        className={classes.dot}
        style={{ ...commonProps, backgroundColor: color }}
      />
    </div>
  );
}

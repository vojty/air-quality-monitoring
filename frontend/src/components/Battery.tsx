import styles from "./Battery.module.css";

type Props = {
  percentage: number;
};

function getColor(percentage: number) {
  if (percentage >= 40) {
    return "#73ad21";
  }
  if (percentage >= 20) {
    return "#ffd148";
  }
  return "#ff7166";
}

export function Battery({ percentage }: Props) {
  return (
    <div className={styles.container}>
      <div className={styles.outer}>
        <div
          className={styles.level}
          style={{ width: `${percentage}%`, background: getColor(percentage) }}
        />
        <div className={styles.percentage}>{percentage.toFixed(1)} %</div>
      </div>
      <div className={styles.bump} />
    </div>
  );
}

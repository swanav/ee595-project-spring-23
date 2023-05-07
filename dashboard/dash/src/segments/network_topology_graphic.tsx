import React from "react";

export default function NetworkTopology(props: { num_flows: number }) {

  const { num_flows} = props;

  const nodeRadius = 10; // radius of the nodes
  const nodeSpacing = 30; // spacing between nodes
  const centerX = 250; // X coordinate of the center of the network
  const centerY = 150; // Y coordinate of the center of the network

  // calculate the number of nodes on each side of the network
  const numNodesPerSide = num_flows;

  // calculate the coordinates of the nodes on the left side of the network
  const leftNodeCoords = [...Array(numNodesPerSide)].map((_, i) => {
    const x = centerX - nodeSpacing * (i + 1);
    const y = centerY;
    return [x, y];
  });

  // calculate the coordinates of the nodes on the right side of the network
  const rightNodeCoords = [...Array(numNodesPerSide)].map((_, i) => {
    const x = centerX + nodeSpacing * (i + 1);
    const y = centerY;
    return [x, y];
  });

  return (
    <svg width="500" height="300">
      {/* draw the left side nodes */}
      {leftNodeCoords.map(([x, y], i) => (
        <circle key={i} cx={x} cy={y} r={nodeRadius} fill="blue" />
      ))}

      {/* draw the right side nodes */}
      {rightNodeCoords.map(([x, y], i) => (
        <circle key={i + numNodesPerSide} cx={x} cy={y} r={nodeRadius} fill="blue" />
      ))}

      {/* draw the left side connections */}
      {leftNodeCoords.map(([x, y], i) => (
        <line key={i} x1={x} y1={y} x2={centerX - nodeSpacing} y2={centerY} stroke="black" strokeWidth="2" />
      ))}

      {/* draw the right side connections */}
      {rightNodeCoords.map(([x, y], i) => (
        <line key={i + numNodesPerSide} x1={x} y1={y} x2={centerX + nodeSpacing} y2={centerY} stroke="black" strokeWidth="2" />
      ))}

      {/* draw the center connection */}
      <line x1={centerX - nodeSpacing} y1={centerY} x2={centerX + nodeSpacing} y2={centerY} stroke="black" strokeWidth="2" />

      {/* draw the left side router */}
      <rect x={centerX - nodeSpacing - nodeRadius} y={centerY - nodeRadius} width={nodeRadius * 2} height={nodeRadius * 2} fill="black" />

      {/* draw the right side router */}
      <rect x={centerX + nodeSpacing - nodeRadius} y={centerY - nodeRadius} width={nodeRadius * 2} height={nodeRadius * 2} fill="black" />
    </svg>
  );
}

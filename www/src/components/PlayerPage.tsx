import { useEffect } from "react";
import AudioPlayerControl from "./AudioPlayerControl";
import AudioSelectionList from "./AudioSelectionList";
import { Box } from "@mui/material";

const PlayerPage = () => {

  // load config
  useEffect(() => {
    console.log("useEffect");
  })

  return (
    <>
      <h1>Player</h1>
        <AudioPlayerControl />
      <AudioSelectionList />
    </>
  );
}

export default PlayerPage;
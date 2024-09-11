import React, { useContext, useEffect, useState } from 'react';
import { WebsocketContext } from './WebsocketProvider';
import PlayCircleIcon from '@mui/icons-material/PlayCircle';
import PauseCircleFilledIcon from '@mui/icons-material/PauseCircleFilled';
import StopCircleIcon from '@mui/icons-material/StopCircle';
import { Box, IconButton, Slider, Stack } from '@mui/material';
import { VolumeDown, VolumeUp } from '@mui/icons-material';

const AudioPlayerControl = () => {
    // const dispatch = useDispatch();
    const [ready, val, send] = useContext(WebsocketContext);
    const [playing, setPlaying] = useState<boolean>(false);
    const [paused, setPaused] = useState<boolean>(false);
    const [title, setTitle] = useState<string>("");
    const [volume, setVolume] = useState<number>(0);

    const clickResume = () => {
        console.log("Resume");
        send({"action": "resume"})
    }

    const clickPause = () => {
        console.log("Pause");
        send({"action": "pause"})
    }

    const clickStop = () => {
        console.log("Stop")
        send({"action": "stop"})
    }

    const changeVolume = (event: Event, newValue: number | number[]) => {
        console.log("Change volume to ", newValue)
        if (typeof newValue === "number") {
            setVolume(newValue);
            send({"action": "volume", "volume": newValue})
        }
    }

    const changeVolumeStep = (step: number) => {
        let newVolume = Math.max(0, Math.min(1, volume + step))
        setVolume(newVolume)
        send({"action": "volume", "volume": newVolume})
    }

    useEffect(() => {
        if (val) {
            setPlaying(val?.player.playing);
            setPaused(val?.player.paused);
            setTitle(val?.player.title);
            setVolume(val.player.volume);
        }
    }, [val]);

    return (
        <Box sx={{ width: 200, margin: '20px auto' }}>
            <div>
                {paused || playing ?
                    <p>{title}</p>
                    :
                    <span></span>
                }
            </div>
            <div>
                {
                    paused || !playing ?
                        <IconButton onClick={clickResume}>
                            <PlayCircleIcon fontSize='large' />
                        </IconButton>
                        :
                        <IconButton onClick={clickPause}>
                            <PauseCircleFilledIcon fontSize='large' />
                        </IconButton>
                }
                <IconButton onClick={clickStop}>
                    <StopCircleIcon fontSize='large' />
                </IconButton>
            </div>
            <div>
                <Stack spacing={2} direction='row' sx={{ mb: 1 }} alignItems='center'>
                    <IconButton onClick={() => { changeVolumeStep(-0.1) }}>
                        <VolumeDown />
                    </IconButton>
                    <Slider aria-label='volume' value={volume} min={0} max={1} step={0.05} onChange={changeVolume} />
                    <IconButton onClick={() => { changeVolumeStep(0.1) }}>
                        <VolumeUp />
                    </IconButton>
                </Stack>

            </div>
        </Box>
    )
};

export default AudioPlayerControl;
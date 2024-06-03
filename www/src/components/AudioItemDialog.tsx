import React, { Dispatch, SetStateAction, useContext, useEffect, useState } from 'react';
import { AudioSource } from './AudioSource';
import { Alert, Box, Button, CircularProgress, Dialog, DialogActions, DialogContent, DialogContentText, DialogTitle, Slider, TextField, styled } from '@mui/material';
import CloudUploadIcon from '@mui/icons-material/CloudUpload';
import { URL_API_SONGS_UPDATE } from './ApiAccess';



interface AudioItemDialogContentProps {
    item: AudioSource;
}

const StreamDialogContent = (props: AudioItemDialogContentProps) => {
    const { item, ...other } = props
    const [stream, setStream] = React.useState<AudioSource>(item);

    return <>
        <DialogTitle>Add stream</DialogTitle>
        <DialogContent>
            <TextField
                autoFocus
                required
                margin="dense"
                id="name"
                name="name"
                label="Name"
                type="string"
                fullWidth
                variant="standard"
                defaultValue={stream.name}
            />
            <TextField
                autoFocus
                required
                margin="dense"
                id="url"
                name="url"
                label="Url"
                type="string"
                fullWidth
                variant="standard"
                defaultValue={stream.url}
            />
        </DialogContent>
    </>
}

const FileDialogContent = (props: AudioItemDialogContentProps) => {
    const { item, ...other } = props
    const VisuallyHiddenInput = styled('input')({
        clip: 'rect(0 0 0 0)',
        clipPath: 'inset(50%)',
        height: 1,
        overflow: 'hidden',
        position: 'absolute',
        bottom: 0,
        left: 0,
        whiteSpace: 'nowrap',
        width: 1,
    });

    return <>
        <DialogTitle>Upload file</DialogTitle>
        <DialogContent>
            <Button
                component="label"
                role={undefined}
                variant="contained"
                tabIndex={-1}
                startIcon={<CloudUploadIcon />}
            >
                Upload file
                <VisuallyHiddenInput type="file" />
            </Button>
        </DialogContent>
    </>
}


const FMDialogContent = (props: AudioItemDialogContentProps) => {
    const { item, ...other } = props
    const [freq, setFreq] = React.useState<number>(parseFloat(item.url.toString()));


    const changeFrequencySlider = (event: Event, value: number | number[], activeThumb: number) => {
        if (typeof value === "number") {
            setFreq(value)
        }
    }

    const changeFrequencyInput = (event: React.ChangeEvent<HTMLInputElement>) => {
        setFreq(Math.min(110, Math.max(85, parseFloat(event.target.value))))
    }

    return <>
        <DialogTitle>Add FM radio</DialogTitle>
        <DialogContent>
            <TextField
                autoFocus
                required
                margin="dense"
                id="name"
                name="name"
                label="Name"
                type="string"
                fullWidth
                variant="standard"
                defaultValue={item.name}
            />
            <TextField
                autoFocus
                required
                margin="dense"
                id="url"
                name="url"
                label="Frequency (MHz)"
                type="number"
                fullWidth
                variant="standard"
                value={freq}
                onChange={changeFrequencyInput}
            />
            <Slider min={85} max={110} value={freq} aria-label="Default" valueLabelDisplay="auto" onChange={changeFrequencySlider} step={0.05} />
        </DialogContent>
    </>
}

interface AudioItemDialogProps {
    open: boolean;
    setOpened: Dispatch<SetStateAction<boolean>>;
    item: AudioSource;
    isNew: boolean;
    setLoaded: Dispatch<SetStateAction<boolean>>;
}

export const AudioItemDialog = (props: AudioItemDialogProps) => {
    const { open, setOpened, item, isNew, setLoaded, ...other } = props
    const [audioItem, setAudioItem] = useState<AudioSource>(item);
    const [dialogContent, setDialogContent] = useState<JSX.Element>(<FileDialogContent item={audioItem} />);
    const [submitting, setSubmitting] = useState<boolean>(false);
    const [errorMsg, setErrorMsg] = useState<string>("");

    const handleClose = () => {
        setOpened(false);
        setErrorMsg("");
        setSubmitting(false);
    };

    const handleSubmit = (event: React.FormEvent<HTMLFormElement>) => {
        event.preventDefault();
        setErrorMsg("")
        setSubmitting(true);

        if (audioItem.type !== "file") {
            const formData = new FormData(event.currentTarget);
            const formJson = Object.fromEntries((formData as any).entries());
            console.log("Is new: ", isNew)
            console.log(formJson);

            let song = {
                name: formJson.name,
                url: formJson.url,
                type: audioItem.type
            }

            AbortSignal.timeout ??= function timeout(ms) {
                const ctrl = new AbortController()
                setTimeout(() => ctrl.abort(), ms)
                return ctrl.signal
            }

            let submitData = {
                "action": "addStream",
                "stream": song
            }

            fetch(URL_API_SONGS_UPDATE, {
                method: "POST",
                headers: {
                    "Accept": "application/json",
                    "Content-Type": "application/x-www-form-urlencoded",
                },
                body: JSON.stringify(submitData),
                signal: AbortSignal.timeout(10000)
            }).then((response) => {
                if (response.status != 200) {
                    console.log("Error", response);
                    setErrorMsg(response.text.toString())
                    setSubmitting(false);
                } else {
                    console.log("added");
                    setLoaded(false);
                    handleClose();
                }
            }).catch((error: any) => {
                console.log("Error", error);
                setErrorMsg(error.toString())
                setSubmitting(false);
            })
        }
    }

    useEffect(() => {
        switch (audioItem.type) {
            case "file":
                setDialogContent(<FileDialogContent item={audioItem} />)
                break;
            case "stream":
                setDialogContent(<StreamDialogContent item={audioItem} />)
                break;
            case "fm":
                setDialogContent(<FMDialogContent item={audioItem} />)
                break;
        }
    }, []);

    return (
        <React.Fragment>
            <Dialog
                open={open}
                onClose={handleClose}
                PaperProps={
                    {
                        component: 'form',
                        onSubmit: handleSubmit
                    }
                }
            >
                {submitting ?
                    <DialogContent>
                        <Box alignContent="center"><CircularProgress /></Box>
                    </DialogContent>
                    : dialogContent
                }
                {errorMsg.length > 0 && (
                    <Alert severity="error">{errorMsg}</Alert>
                )}
                <DialogActions>
                    <Button onClick={handleClose}>Cancel</Button>
                    <Button type="submit">Add</Button>
                </DialogActions>
            </Dialog>
        </React.Fragment>
    );
}

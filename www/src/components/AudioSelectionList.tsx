import React, { Dispatch, SetStateAction, useEffect, useState } from 'react';
import { Box, CircularProgress, Fab, IconButton, Paper, Tab, Table, TableBody, TableCell, TableContainer, TableHead, TableRow, Tabs, Typography } from '@mui/material';
import { AudioSource } from './AudioSource';
import { URL_API_PLAYBACK, URL_API_SONGS, URL_API_SONGS_UPDATE } from './ApiAccess';
import EditIcon from '@mui/icons-material/Edit';
import AddIcon from '@mui/icons-material/Add';
import DeleteIcon from '@mui/icons-material/Delete';
import PlayArrowIcon from '@mui/icons-material/PlayArrow';
import { AudioItemDialog } from './AudioItemDialog';
import { request } from './Request';


interface AudioFileItemProps {
    item: AudioSource;
    setLoaded: Dispatch<SetStateAction<boolean>>;
    editAudioItem: Function;
}

const AudioFileItem = (props: AudioFileItemProps) => {
    const { item, setLoaded, editAudioItem, ...other } = props

    const playItem = () => {
        console.log("Playing item", item);

        let formData = {
            "action": "play",
            "url": item.url,
            "name": item.name,
            "volume": 0.6
        }

        fetch(URL_API_PLAYBACK, {
            method: "POST",
            headers: {
                "Accept": "application/json",
                "Content-Type": "application/x-www-form-urlencoded",
            },
            body: JSON.stringify(formData),
        }).then((response) => {
            if (response.status != 200) {
                console.log("Error", response);
            } else {
                console.log("Saved");
            }
        }).catch((error: any) => {
            console.log("Error fetching audio files", error);
        })
    }

    const editItem = () => {
        editAudioItem(item);
        // setLoaded(false);
    }

    const deleteItem = () => {
        if (!window.confirm("Do you really want to delete " + item.name + "?")) {
            return
        }

        console.log("delete item", item);

        let formData = {
            "action": "delete",
            "song": {
                name: item.name,
                url: item.url ? item.url : "",
                type: item.type
            }
        }

        fetch(URL_API_SONGS_UPDATE, {
            method: "POST",
            headers: {
                "Accept": "application/json",
                "Content-Type": "application/x-www-form-urlencoded",
            },
            body: JSON.stringify(formData),
        }).then((response) => {
            if (response.status != 200) {
                console.log("Error", response);
            } else {
                console.log("deleted");
                setLoaded(false);
            }
        }).catch((error: any) => {
            console.log("Error", error);
        })
    }

    return (
        <TableRow
            key={item.name}
            sx={{ '&:last-child td, &:last-child th': { border: 0 } }}
        >
            <TableCell component="th" scope="row">
                {item.name}
            </TableCell>
            {item.type === "stream" ? (
                <TableCell align="left">
                    <a href={item.url.toString()}>{item.url.toString().substring(0, 30)}...</a></TableCell>
            ) : (
                <TableCell align="right">{item.url}</TableCell>
            )
            }
            {item.type === "file" && (
                <TableCell align="right">{item.size ? (item.size / 1024 / 1024).toFixed(2) : 0} Mb</TableCell>
            )
            }
            <TableCell size="small" align="right">
                {/* {item.type !== "file" && (
                    <IconButton onClick={editItem}><EditIcon /></IconButton>
                )
                } */}
                <IconButton onClick={deleteItem}><DeleteIcon /></IconButton>
                <IconButton onClick={playItem}><PlayArrowIcon /></IconButton>
            </TableCell>
        </TableRow>
    )
}

interface TabPanelProps {
    children?: React.ReactNode;
    index: number;
    value: number;
    items: Array<AudioSource>;
    type: "file" | "stream" | "fm";
    setLoaded: Dispatch<SetStateAction<boolean>>;
}

const AudioItemList = (props: TabPanelProps) => {
    const getInitializedAudioItem = (type: "file" | "stream" | "fm") => {
        switch (type) {
            case "file":
                return { name: "", url: "", type: type, size: undefined }
            case "stream":
                return { name: "new stream", url: "https://", type: type, size: undefined }
            case "fm":
                return { name: "new radio", url: 100, type: type, size: undefined }
        }
    }

    const { children, value, index, items, type, setLoaded, ...other } = props;
    const [isNew, setIsNew] = React.useState<boolean>(true);
    const [selectedItem, setSelectedItem] = React.useState<AudioSource>(getInitializedAudioItem(type));
    const [dialogOpen, setDialogOpen] = React.useState<boolean>(false);

    let itemsFiltered = items.filter((value: AudioSource) => { return value.type === type })

    const addAudioItem = () => {
        console.log("Add audio item of type " + type);
        setIsNew(true);
        setSelectedItem(getInitializedAudioItem(type))
        setDialogOpen(true);
    }

    const editAudioItem = (item: AudioSource) => {
        console.log("edit item ", item)
        setIsNew(false);
        setSelectedItem(item);
        setDialogOpen(true);
    }

    return (
        <div
            role="tabpanel"
            hidden={value !== index}
            id={`simple-tabpanel-${index}`}
            aria-labelledby={`simple-tab-${index}`}
            {...other}
        >
            {value === index && (
                <Box sx={{ p: 1 }}>
                    <TableContainer component={Paper}>
                        <Table sx={{ minWidth: 300 }} size='small' aria-label="simple table">
                            <TableBody>
                                {
                                    itemsFiltered.map(
                                        (item) => (
                                            <AudioFileItem item={item} setLoaded={setLoaded} editAudioItem={editAudioItem} />
                                        )
                                    )
                                }
                            </TableBody>
                        </Table>
                    </TableContainer>
                </Box>
            )}
            <Fab color="primary" aria-label="add" onClick={addAudioItem}>
                <AddIcon />
            </Fab>
            <AudioItemDialog open={dialogOpen} setOpened={setDialogOpen} item={selectedItem} isNew={isNew} setLoaded={setLoaded}/>
        </div>
    )
}

const AudioSelectionList = () => {
    const [tabPage, setTabPage] = React.useState(0);
    const [audioSources, setAudioSources] = useState<Array<AudioSource>>([]);
    const [audioSourcesLoaded, setAudioSourcesLoaded] = useState<boolean>(false);
    const [audioSourcesLoadedError, setAudioSourcesLoadedError] = useState<boolean>(false);

    const handleChange = (event: React.SyntheticEvent, newValue: number) => {
        setTabPage(newValue);
    };

    useEffect(() => {
        request<Array<AudioSource>>(URL_API_SONGS).then(songs => {
            setAudioSources(songs);
            setAudioSourcesLoaded(true);
            console.log("Loaded songs!", audioSources)
        }).catch(error => {
            console.log("Error while loiading audio sources", error);
            setAudioSourcesLoadedError(true);
        })
    }, [audioSourcesLoaded]);

    return (
        <>
            <Box sx={{ borderBottom: 1, borderColor: 'divider' }}>
                <Tabs value={tabPage} onChange={handleChange} aria-label="audio files">
                    <Tab label="File" />
                    <Tab label="Web stream" />
                    <Tab label="FM Radio" />
                </Tabs>
            </Box>
            {audioSourcesLoaded ? (
                <>
                    <AudioItemList value={tabPage} index={0} items={audioSources} type={"file"} setLoaded={setAudioSourcesLoaded} />
                    <AudioItemList value={tabPage} index={1} items={audioSources} type={"stream"} setLoaded={setAudioSourcesLoaded} />
                    <AudioItemList value={tabPage} index={2} items={audioSources} type={"fm"} setLoaded={setAudioSourcesLoaded} />
                </>
            )
                : <Box><CircularProgress/></Box>
            }
        </>
    )
};

export default AudioSelectionList;
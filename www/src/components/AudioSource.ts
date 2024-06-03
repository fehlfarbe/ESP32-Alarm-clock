export interface AudioSource {
    name: string;
    url: string | number;
    type: "file" | "stream" | "fm";
    size: number | undefined;
  }
import { useEffect } from "react";

const ConfigPage = () => {

  // load config
  useEffect(() => {
    console.log("useEffect");
  })

  const log = (type: string) => console.log.bind(console, type);

  return (
    <>
      <h1>ConfigPage</h1>
    </>
  );
}

export default ConfigPage;
export function request<TResponse>(
    url: string,
    // `RequestInit` is a type for configuring 
    // a `fetch` request. By default, an empty object.
    config: RequestInit = {},
    timeout: number = 30000
): Promise<TResponse> {
    AbortSignal.timeout ??= function timeout(ms) {
        const ctrl = new AbortController()
        setTimeout(() => ctrl.abort(), ms)
        return ctrl.signal
    }
    config["signal"] = AbortSignal.timeout(timeout);
    // Inside, we call the `fetch` function with 
    // a URL and config given:
    return fetch(url, config)
        // When got a response call a `json` method on it
        .then((response) => response.json())
        // and return the result data.
        .then((data) => data as TResponse);
}
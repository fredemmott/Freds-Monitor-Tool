# Freds Monitor Tool

This is a set of command-line tools to save and restore monitor configuration profiles; a profile contains a list of which monitors are enabled, their layout, their resolution, among other settings.

## Quick Start

64-Bit Windows 10 or newer is required.

1. Download [the latest release](https://github.com/fredemmott/Freds-Monitor-Tool/releases/latest)
1. Extract the zip somewhere handy
1. Run `fmt-create-profile "First Profile"`
1. Change your display settings as desired, e.g. by disabling or enabling monitors
1. Run `fmt-create-profile "Second Profile"`
1. Run `fmt-apply-profile "First Profile"` to restore the previous settings
1. Run `fmt-apply-profile "Second Profile"` to switch to the second settings
1. Run `fmt-list-profiles` to view a list of all your profiles

You can use any profile name you wish - just remember the quotes if there's spaces or other special characters.

You can run these programs in three ways:
- add the folder your extract them to to the `%PATH%` environment variable
- run them from a terminal inside that folder
- run them with the full path, e.g. `C:\Users\fred\fmt\fmt-apply-profile.exe`

The programs can be ran from a terminal, double-clicked on, or launched by other programs, e.g. Voice Attack or an Elgato Stream Deck.

Profiles are stored in `%LOCALAPPDATA%\Freds Monitor Tool\Profiles`.

### Deleting Profiles

Delete the corresponding file from `%LOCALAPPDATA%\Freds Monitor Tool\Profiles`

### Renaming Profiles

1. Apply the profile
2. Delete the profile
3. Create a new profile with the desired name

### Using Profiles In Other Locations

1. `fmt-create-profile "Profile Name" --path MyProfile.json`
2. `fmt-apply-profile --path MyProfile.json`

## Support or Help

Run any of these exe files with no arguments (or with `--help`) to see advanced usage information.

I make this for myself, and share it in case others find it useful; I do not currently have the time or energy for:

- help or support
- bug reports
- feature requests

This project is distributed as-is; no help or support is offered or available. **Do not ask me for help, bug fixes, or features.**

### Bug Reports

See above.

### Feature Requests

See above.

It is reasonably likely that a future version will include a system tray feature; there is no ETA, news, or updates. Do not ask me for ETA, news, or updates.

Small, standalone features (e.g. at the scale of 'extend to this new monitor') are likely to be accepted if [contributed](CONTRIBUTING.md).

## License

Copyright 2024, Fred Emmott

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted, provided that the above copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED “AS IS” AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

# NAUL Desktop
A desktop program for the NAUL operating system.

## Features
- Launchable taskbar icons
- Taskbar icons for windows
- IPC API
- Window events
- Minimising windows
- Dragging windows
- Wallpaper stretching

## Functions
Calls can be made via sendMessage with the name "desktop" with the following args:
| Function        | Arg1               | Arg2             | Arg3              | Return value |
| --------------- | ------------------ | ---------------- | ----------------- | ------------ |
| `createWindow`  | CREATE_WINDOW (0)  | `uint32_t width` | `uint32_t height` | `Window*`    |
| `destroyWindow` | DESTROY_WINDOW (1) | `Window* window` | N/A (0)           | N/A (0)      |

## Types
The window struct will contain the following data:
| Type                | Name           | Default value   | Description                          |
| ------------------- | -------------- | --------------- | ------------------------------------ |
| `uint32_t`          | width          | `width + 20`    | Actual window width                  |
| `uint32_t`          | height         | `height + 57`   | Actual window height                 |
| `uint32_t`          | x              | Middle          | X position on the screen             |
| `uint32_t`          | y              | Center          | Y position on the screen             |
| `bool`              | minimised      | `false`         | Whether the window is shown          |
| `bool`              | lockMouse      | `false`         | Whether to lock the mouse upon click |
| `uint32_t`          | bufferWidth    | `width`         | Width of the buffer                  |
| `uint32_t`          | bufferHeight   | `height`        | Height of the buffer                 |
| `uint32_t*`         | buffer         | N/A             | Pointer to the buffer                |
| `uint8_t`           | eventsHead     | `0`             | Head of the events list              |
| `uint8_t`           | eventsTail     | `0`             | Tail of the events list              |
| `WindowEvent[256]`  | events         | Uninitialised   | Window events list                   |
| `char[64]`          | title          | `""`            | Title of the window                  |
| `uint32_t[24 * 24]` | icon           | Black square    | Icon of the window                   |

The WindowEvent type is a union of the following structs:
#### Close event
| Type      | Name     | Description                            |
| --------- | -------- | -------------------------------------- |
| `uint8_t` | type     | Type of event (always CLOSE_EVENT (0)) |

#### Key event
| Type      | Name     | Description                             |
| --------- | -------- | --------------------------------------- |
| `uint8_t` | type     | Type of event (always KEY_EVENT (1))    |
| `uint8_t` | scancode | PS/2 scancode of the key                |
| `bool`    | pressed  | Whether the key was pressed or released |

#### Mouse move event
| Type      | Name     | Description                                              |
| --------- | -------- | -------------------------------------------------------- |
| `uint8_t` | type     | Type of event (always MOUSE_MOVE_EVENT (2))              |
| `int32_t` | x        | X position relative to the window (or delta when locked) |
| `int32_t` | y        | Y position relative to the window (or delta when locked) |

#### Mouse click event
| Type      | Name     | Description                                  |
| --------- | -------- | -------------------------------------------- |
| `uint8_t` | type     | Type of event (always MOUSE_CLICK_EVENT (3)) |
| `bool`    | left     | Whether it was a left or right click         |
| `bool`    | pressed  | Whether the button was pressed or released   |

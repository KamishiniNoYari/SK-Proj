import tkinter as tk
import socket
import time


def connect(sock, connection_widgets, refresh_flags):
    try:
        host = connection_widgets['host_entry'].get()
        port = int(connection_widgets['port_entry'].get())
        sock.connect((host, port))
        sock.send(b'CONNECTED')

        connection_widgets['host'] = host
        connection_widgets['port'] = port

        connection_widgets['connected'] = True
        refresh_flags['connection_widgets'] = True
    except Exception as e:
        print(e)


def disconnect(sock, connection_widgets, refresh_flags):
    try:
        sock.close()
        connection_widgets['connected'] = False

        refresh_flags['connection_widgets'] = True
        refresh_flags['socket_reset'] = True
    except Exception as e:
        print(e)


def start_game(sock, refresh_flags, start_game_widgets):
    try:
        sock.send(b'START')

        start_game_widgets['started'] = True
        refresh_flags['start_game_widgets'] = True
    except Exception as e:
        print(e)


def init_connection_widgets(root, sock, refresh_flags):
    # Not connected widgets
    host_label = tk.Label(root, text="Host: ")
    host_entry = tk.Entry(root)
    port_label = tk.Label(root, text="Port: ")
    port_entry = tk.Entry(root)
    connect_btn = tk.Button(
        root,
        command=lambda: connect(sock, connection_widgets, refresh_flags),
        text="Connect"
    )

    # Connected widgets
    connected_label = tk.Label(root)
    disconnect_btn = tk.Button(
        root,
        command=lambda: disconnect(sock, connection_widgets, refresh_flags),
        text="Disconnect"
    )

    connection_widgets = {
        'host_label': host_label,
        'host_entry': host_entry,
        'port_label': port_label,
        'port_entry': port_entry,
        'connect_btn': connect_btn,
        'connected': False,

        'connected_label': connected_label,
        'disconnect_btn': disconnect_btn,
        'host': '',
        'port': ''
    }

    host_label.grid(column=0, row=0)
    host_entry.grid(column=1, row=0)
    port_label.grid(column=2, row=0)
    port_entry.grid(column=3, row=0)
    connect_btn.grid(column=4, row=0)

    return connection_widgets


def refresh_connection_widgets(connection_widgets, refresh_flags, start_game_widgets):
    keys_not_connected = ['host_label', 'host_entry', 'port_label', 'port_entry', 'connect_btn']
    keys_connected = ['connected_label', 'disconnect_btn']

    if connection_widgets['connected']:
        for key in keys_not_connected:
            connection_widgets[key].grid_forget()

        connection_widgets['connected_label'].config(
            text=f'Connected to {connection_widgets["host"]}:{[connection_widgets["port"]]}'
        )
        for idx, key in enumerate(keys_connected):
            connection_widgets[key].grid(column=idx, row=0)

    else:
        for key in keys_connected:
            connection_widgets[key].grid_forget()

        for idx, key in enumerate(keys_not_connected):
            connection_widgets[key].grid(column=idx, row=0)

        start_game_widgets['started'] = False

    refresh_flags['start_game_widgets'] = True
    refresh_flags['connection_widgets'] = False


def init_start_game_widgets(root, sock, refresh_flags):
    start_btn = tk.Button(
        root,
        command=lambda: start_game(sock, refresh_flags, start_game_widgets),
        state=tk.DISABLED,
        text="START GAME"
    )

    start_game_widgets = {
        'start_btn': start_btn,
        'started': False
    }

    start_btn.place(relx=0.5, y=50, anchor=tk.CENTER)

    return start_game_widgets


def refresh_start_game_widgets(start_game_widgets, connection_widgets, refresh_flags):
    if connection_widgets['connected'] and not start_game_widgets['started']:
        start_game_widgets['start_btn'].config(state=tk.ACTIVE)
    else:
        start_game_widgets['start_btn'].config(state=tk.DISABLED)

    refresh_flags['start_game_widgets'] = False


def socket_reset(connection_widgets, start_game_widgets, refresh_flags):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    start_game_widgets['start_btn'].config(
        command=lambda: start_game(sock, refresh_flags, start_game_widgets)
    )
    connection_widgets['connect_btn'].config(
        command=lambda: connect(sock, connection_widgets, refresh_flags)
    )
    connection_widgets['disconnect_btn'].config(
        command=lambda: disconnect(sock, connection_widgets, refresh_flags)
    )

    refresh_flags['socket_reset'] = False

    return sock


def destroy_window(refresh_flags):
    print('CLOSING')
    refresh_flags['running'] = False


def main():
    root = tk.Tk()
    running = True
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    refresh_flags = {
        'connection_widgets': False,
        'start_game_widgets': False,
        'socket_reset': False,

        'running': True
    }

    root.geometry('600x400')
    root.title('Client')
    root.protocol('WM_DELETE_WINDOW', lambda: destroy_window(refresh_flags))

    connection_widgets = init_connection_widgets(root, sock, refresh_flags)
    start_game_widgets = init_start_game_widgets(root, sock, refresh_flags)

    while refresh_flags['running']:
        root.update()
        root.update_idletasks()

        if refresh_flags['connection_widgets']:
            refresh_connection_widgets(connection_widgets, refresh_flags, start_game_widgets)

        if refresh_flags['start_game_widgets']:
            refresh_start_game_widgets(start_game_widgets, connection_widgets, refresh_flags)

        if refresh_flags['socket_reset']:
            sock = socket_reset(connection_widgets, start_game_widgets, refresh_flags)

        time.sleep(0.01)

    # root.mainloop()


if __name__ == '__main__':
    main()

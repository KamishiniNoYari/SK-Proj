import tkinter as tk
import socket
import time
import math
import threading


def connect(sock, connection_widgets, refresh_flags, listener_thread):
    try:
        host = connection_widgets['host_entry'].get()
        port = int(connection_widgets['port_entry'].get())
        sock.connect((host, port))

        connection_widgets['host'] = host
        connection_widgets['port'] = port

        connection_widgets['connected'] = True
        refresh_flags['connection_widgets'] = True

        listener_thread.start()
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
        sock.send(b'START\n')

        start_game_widgets['started'] = True
        refresh_flags['start_game_widgets'] = True
    except Exception as e:
        print(e)


def send_answer(sock, input_fields_widgets, refresh_flags, root):
    error = validate_inputs(input_fields_widgets)
    if not error:
        try:
            country = input_fields_widgets['country_entry'].get()
            city = input_fields_widgets['city_entry'].get()
            animal = input_fields_widgets['animal_entry'].get()
            plant = input_fields_widgets['plant_entry'].get()
            name = input_fields_widgets['name_entry'].get()
            answer = f'AN {country};{city};{animal};{plant};{name};\n'.encode('utf-8')
            print(answer)

            input_fields_widgets['send_btn'].config(
                state=tk.DISABLED
            )

            sock.send(answer)
        except Exception as e:
            print(e)

    else:
        popup = tk.Toplevel(root)
        popup.title('Error')
        position_x = root.winfo_x() + 450
        position_y = root.winfo_y() + 350
        popup.geometry(f'+{position_x}+{position_y}')

        label = tk.Label(popup, text='Remove all semicolons from inputs')
        label.grid(column=0, row=0)

        accept_btn = tk.Button(popup, command=popup.destroy, text='OK')
        accept_btn.grid(column=0, row=1)

        print(error)


def init_connection_widgets(root, sock, refresh_flags, threads):
    # Not connected widgets
    host_label = tk.Label(root, text="Host: ")
    host_entry = tk.Entry(root)
    port_label = tk.Label(root, text="Port: ")
    port_entry = tk.Entry(root)
    connect_btn = tk.Button(
        root,
        command=lambda: connect(
            sock,
            connection_widgets,
            refresh_flags,
            threads['listener_thread']
        ),
        text="Connect"
    )
    host_entry.insert(0, 'localhost')
    port_entry.insert(0, '8000')

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

    letter_label = tk.Label(root, text='Letter: ', font=('font', 18))
    letter_label.place(relx=0.06, rely=0.19)
    points_label = tk.Label(root, text='Points: ', font=('font', 18))
    points_label.place(relx=0.25, rely=0.19)
    place_label = tk.Label(root)

    start_game_widgets = {
        'start_btn': start_btn,
        'started': False,

        'letter_label': letter_label,
        'letter': '',
        'points_label': points_label,
        'points': '',
        'place_label': place_label,
        'place': ''
    }

    start_btn.place(relx=0.5, rely=0.12, anchor=tk.CENTER)

    return start_game_widgets


def refresh_start_game_widgets(start_game_widgets, connection_widgets, refresh_flags):
    if connection_widgets['connected'] and not start_game_widgets['started']:
        start_game_widgets['start_btn'].config(state=tk.ACTIVE)
    else:
        start_game_widgets['start_btn'].config(state=tk.DISABLED)

    refresh_flags['input_fields_widgets'] = True
    refresh_flags['start_game_widgets'] = False


def init_input_fields_widgets(root, sock, refresh_flags):
    country_label = tk.Label(root, text='Country')
    country_entry = tk.Entry(root)
    city_label = tk.Label(root, text='City')
    city_entry = tk.Entry(root)
    animal_label = tk.Label(root, text='Animal')
    animal_entry = tk.Entry(root)
    plant_label = tk.Label(root, text='Plant')
    plant_entry = tk.Entry(root)
    name_label = tk.Label(root, text='Name')
    name_entry = tk.Entry(root)

    send_btn = tk.Button(
        root,
        # command=lambda: print('IMPLEMENT SEND'),
        command=lambda: send_answer(sock, input_fields_widgets, refresh_flags, root),
        text='SEND',
        state=tk.DISABLED
    )

    input_fields_widgets = {
        'country_label': country_label,
        'country_entry': country_entry,
        'city_label': city_label,
        'city_entry': city_entry,
        'animal_label': animal_label,
        'animal_entry': animal_entry,
        'plant_label': plant_label,
        'plant_entry': plant_entry,
        'name_label': name_label,
        'name_entry': name_entry,
        'send_btn': send_btn,

        'sent': False
    }

    rely_labels = 0.25
    rely_entries = 0.3
    relx_distance = 1 / 5.0
    margin_left_labels = 0.05
    margin_left_entries = 0.02

    for idx, key in enumerate(input_fields_widgets):
        if idx < 10:
            if idx % 2 == 0:
                input_fields_widgets[key].place(
                    relx=margin_left_labels + math.floor(idx / 2) * relx_distance + 0.02,
                    rely=rely_labels
                )
            else:
                input_fields_widgets[key].place(
                    relx=margin_left_entries + math.floor(idx / 2) * relx_distance,
                    rely=rely_entries
                )

    send_btn.place(relx=0.5, rely=0.4, anchor=tk.CENTER)

    return input_fields_widgets


def refresh_input_fields_widgets(input_fields_widgets, start_game_widgets, refresh_flags):
    if start_game_widgets['started'] and not input_fields_widgets['sent']:
        input_fields_widgets['send_btn'].config(
            state=tk.ACTIVE
        )
    else:
        input_fields_widgets['send_btn'].config(
            state=tk.DISABLED
        )
    refresh_flags['input_fields_widgets'] = False


def validate_inputs(input_fields_widgets):
    errors = list()
    for idx, key in enumerate(input_fields_widgets):
        if idx % 2 == 1 and idx < 10:
            entry = input_fields_widgets[key].get()
            if ";" in input_fields_widgets[key].get():
                errors.append([key, '; in entry'])
            print(f'; in {input_fields_widgets[key].get()}: {";" in entry}')

    return errors


def socket_reset(connection_widgets, start_game_widgets, input_fields_widgets, refresh_flags, root, listener_thread):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    start_game_widgets['start_btn'].config(
        command=lambda: start_game(sock, refresh_flags, start_game_widgets)
    )
    connection_widgets['connect_btn'].config(
        command=lambda: connect(sock, connection_widgets, refresh_flags, listener_thread)
    )
    connection_widgets['disconnect_btn'].config(
        command=lambda: disconnect(sock, connection_widgets, refresh_flags)
    )
    input_fields_widgets['send_btn'].config(
        command=lambda: send_answer(sock, input_fields_widgets, refresh_flags, root)
    )

    refresh_flags['socket_reset'] = False

    return sock


def handle_round_start(start_game_widgets, input_fields_widgets, refresh_flags):
    start_game_widgets['start_btn'].config(state=tk.DISABLED)
    input_fields_widgets['send_btn'].config(state=tk.ACTIVE)
    start_game_widgets['letter_label'].config(
        text=f'Letter: {start_game_widgets["letter"]}'
    )
    for idx, key in enumerate(input_fields_widgets):
        if idx < 10 and idx % 2 == 1:
            input_fields_widgets[key].delete(0, 'end')

    refresh_flags['round_started'] = False


def handle_place_and_score(start_game_widgets, input_fields_widgets, refresh_flags):
    if refresh_flags['new_points']:
        start_game_widgets['points_label'].config(
            text=f'Points: {start_game_widgets["points"]}'
        )

        refresh_flags['new_points'] = False
    elif refresh_flags['round_finished']:
        input_fields_widgets['send_btn'].config(
            state=tk.DISABLED
        )

        refresh_flags['round_finished'] = False


def handle_end(start_game_widgets, input_fields_widgets, refresh_flags):
    print('ENDING GAME')
    start_game_widgets['place_label'].config(
        text=f'GAME FINISHED, PLACE {start_game_widgets["place"]}',
        font=('font', 18)
    )
    start_game_widgets['place_label'].place(relx=0.35, rely=0.6)

    for idx, key in enumerate(input_fields_widgets):
        if idx < 10 and idx % 2 == 1:
            input_fields_widgets[key].delete(0, 'end')

    refresh_flags['end'] = False


def listener(sock, connection_widgets, start_game_widgets, refresh_flags):
    counting = False
    # end = False
    while connection_widgets['connected']:
        response = sock.recv(512).decode('utf-8')

        if len(response) == 0:
            break

        response = response.split('\x00')

        letter = ''
        for res in response:
            print(res)
            if len(res) == 0:
                pass
            elif res[0].islower():
                letter = res[0]
                start_game_widgets['letter'] = letter
            elif res[:2] == 'RO':
                refresh_flags['round_started'] = True
            elif res[:2] == 'CP':
                counting = True
            elif res[:3] == 'EOR':
                refresh_flags['round_finished'] = True
            # elif res[:4] == 'END':
                # end = True
            elif res[:5] == 'PLACE':
                start_game_widgets['place'] = res[6]
                refresh_flags['end'] = True
                end = False
            elif counting:
                try:
                    print('GETTING SCORE')
                    _ = int(res)
                    start_game_widgets['points'] = res
                    counting = False
                    refresh_flags['new_points'] = True
                except ValueError:
                    print(f'{res} is not a number')


def destroy_window(refresh_flags):
    print('CLOSING')
    refresh_flags['running'] = False


def main():
    root = tk.Tk()
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    refresh_flags = {
        'connection_widgets': False,
        'start_game_widgets': False,
        'input_fields_widgets': False,
        'socket_reset': False,
        'round_started': False,
        'round_finished': False,
        'new_points': False,
        'end': False,

        'running': True
    }

    root.geometry('1000x800')
    root.title('Client')
    root.protocol('WM_DELETE_WINDOW', lambda: destroy_window(refresh_flags))

    threads = dict()

    connection_widgets = init_connection_widgets(root, sock, refresh_flags, threads)
    start_game_widgets = init_start_game_widgets(root, sock, refresh_flags)
    input_fields_widgets = init_input_fields_widgets(root, sock, refresh_flags)

    listener_thread = threading.Thread(
        target=listener,
        args=(sock, connection_widgets, start_game_widgets, refresh_flags),
        daemon=True
    )
    threads['listener_thread'] = listener_thread

    while refresh_flags['running']:
        root.update()
        root.update_idletasks()

        if refresh_flags['connection_widgets']:
            refresh_connection_widgets(connection_widgets, refresh_flags, start_game_widgets)

        if refresh_flags['start_game_widgets']:
            refresh_start_game_widgets(start_game_widgets, connection_widgets, refresh_flags)

        if refresh_flags['input_fields_widgets']:
            refresh_input_fields_widgets(input_fields_widgets, start_game_widgets, refresh_flags)

        if refresh_flags['socket_reset']:
            sock = socket_reset(
                connection_widgets,
                start_game_widgets,
                input_fields_widgets,
                refresh_flags,
                root,
                listener_thread
            )

        if refresh_flags['round_started']:
            handle_round_start(start_game_widgets, input_fields_widgets, refresh_flags)

        if refresh_flags['round_finished'] or refresh_flags['new_points']:
            handle_place_and_score(start_game_widgets, input_fields_widgets, refresh_flags)

        if refresh_flags['end']:
            handle_end(start_game_widgets, input_fields_widgets, refresh_flags)

        time.sleep(0.01)

    # root.mainloop()


if __name__ == '__main__':
    main()

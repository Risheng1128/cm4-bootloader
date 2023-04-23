import os
import argparse
import utility

if __name__ == '__main__':
    parse = argparse.ArgumentParser()
    parse.add_argument('-p', '--port', help='serial port',
                       default='/dev/ttyACM0')
    args = parse.parse_args()

    # connect serial port
    port = utility.serial_port_setting(args.port)
    os.system('clear')

    curr = 0
    while True:
        # ui initialization
        screen = utility.screen_init()

        # clear screen
        screen.clear()

        # display menu
        utility.print_menu(screen, curr)

        # get user input
        key = screen.getch()

        # select user input
        curr = utility.select_key(port, screen, key, curr)

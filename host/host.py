import argparse
import utility
import command

if __name__ == '__main__':
    parse = argparse.ArgumentParser()
    parse.add_argument('-p', '--port', help='serial port',
                       default='/dev/ttyACM0')
    args = parse.parse_args()

    # connect serial port
    port = utility.serial_port_setting(args.port)

    utility.print_menu()
    while True:
        command_code = input('\n Type the command code here: ')
        if not command_code.isdigit():
            print('please input valid code')
            continue

        command.decode_command_code(port, int(command_code))

# Вспомогательный скрипт для перевода вывода xxd к виду Си-массива

def get_bin():
    with open('bin.txt') as init:
        for line in init:
            str = line[10:49].strip()
            str = str.replace(' ', '')
            s_list = [str[i - 2:i] for i in range(2, len(str) + 1, 2)]
            str = '\"' + '\\x'+'\\x'.join(s_list) + '\"'
            print(str)


if __name__ == '__main__':
    get_bin()

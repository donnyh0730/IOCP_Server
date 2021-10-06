import argparse #아규먼트를 편하게 파싱할수 있게 해주는 라이브러리.
import jinja2 # 템플릿 엔진 라이브러리 코드를 원하는 컨벤션으로 제너레이션 해준다.
import ProtoParser

def main():

    arg_parser = argparse.ArgumentParser(description = 'PacketGenerator')
    arg_parser.add_argument('--path', type=str, default ='C:/Users/DongWoong/source/repos/CPP_Server/Server/Common/Protobuf/bin/Protocol.proto',help = 'proto path')
    arg_parser.add_argument('--output', type=str, default = 'TestPacketHandler',help = 'output file')
    arg_parser.add_argument('--recv', type=str, default = 'C_',help = 'recv convention')
    arg_parser.add_argument('--send', type=str, default = 'S_',help = 'send convention')
    #설명 :  --아규먼트name (인자값), 타입은 스트링, 기본값은 ' ' , help는 설명 즉 인자값으로 들어온 값을 
    #str타입으로 파싱하여 메인함수 아규먼트를 받는다.
    args = arg_parser.parse_args()

    parser = ProtoParser.ProtoParser(1000,args.recv,args.send)
    # --recv항목이 있으므로 따로 변수선언이 없더라도 args.recv이런식으로 접근이 가능하다.
    parser.parse_proto(args.path)

    file_loader = jinja2.FileSystemLoader('Templates')
    env = jinja2.Environment(loader = file_loader)

    template = env.get_template('PacketHandler.h')
    output = template.render(parser = parser, output = args.output)

    f = open(args.output+'.h','w+')
    f.write(output)
    f.close()

    print(output)

    return

if __name__ == '__main__':
    main()
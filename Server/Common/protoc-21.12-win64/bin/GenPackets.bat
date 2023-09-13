pushd %~dp0

protoc.exe -I=./ --cpp_out=./ ./Enum.proto
protoc.exe -I=./ --cpp_out=./ ./Struct.proto
protoc.exe -I=./ --cpp_out=./ ./Protocol.proto

GenPackets.exe --path=./Protocol.proto --output=ClientPacketHandler --recv=S_ --send=C_
GenPackets.exe --path=./Protocol.proto --output=ServerPacketHandler --recv=C_ --send=S_

IF ERRORLEVEL 1 PAUSE

XCOPY /Y Enum.pb.h "../../../GameServer"
XCOPY /Y Enum.pb.cc "../../../GameServer"
XCOPY /Y Struct.pb.h "../../../GameServer"
XCOPY /Y Struct.pb.cc "../../../GameServer"
XCOPY /Y Protocol.pb.h "../../../GameServer"
XCOPY /Y Protocol.pb.cc "../../../GameServer"
XCOPY /Y ServerPacketHandler.h "../../../GameServer"

XCOPY /Y Enum.pb.h "../../../../Client/Source/Client/Network/Protocol"
XCOPY /Y Enum.pb.cc "../../../../Client/Source/Client/Network/Protocol"
XCOPY /Y Struct.pb.h "../../../../Client/Source/Client/Network/Protocol"
XCOPY /Y Struct.pb.cc "../../../../Client/Source/Client/Network/Protocol"
XCOPY /Y Protocol.pb.h "../../../../Client/Source/Client/Network/Protocol"
XCOPY /Y Protocol.pb.cc "../../../../Client/Source/Client/Network/Protocol"
XCOPY /Y ClientPacketHandler.h "../../../../Client/Source/Client"

DEL /Q /F *.pb.h
DEL /Q /F *.pb.cc
DEL /Q /F *.h

PAUSE
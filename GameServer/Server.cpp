#include "Server.h"

#include <QString>
#include <QStringList>

QSemaphore messageBuffer::cardBufferFree(10);
QSemaphore messageBuffer::cardBufferUsed(0);
int messageBuffer::cardBuffer[BUFFERSIZE];
int messageBuffer::cardBufferTop = 0;
int messageBuffer::cardBufferBottom = 0;

QSemaphore messageBuffer::inforBufferReadLocker(0);
QSemaphore messageBuffer::inforBufferWriteLocker(1);
int messageBuffer::inforBuffer;

QSemaphore messageBuffer::batBufferReadLocker(0);
QSemaphore messageBuffer::batBufferWriteLocker(1);
BatInfor messageBuffer::batBuffer;

QSemaphore messageBuffer::msgBufferReadLocker(0);
QSemaphore messageBuffer::msgBufferWriteLocker(1);
QString messageBuffer::msg;
void messageBuffer::init()
{

}

BatInfor messageBuffer::readBatInfor()
{
    messageBuffer::batBufferReadLocker.acquire();;
    BatInfor ans = messageBuffer::batBuffer;
    messageBuffer::batBufferWriteLocker.release();;
    return ans;
}

int messageBuffer::readInfor()
{
    messageBuffer::inforBufferReadLocker.acquire();;
    int ans = messageBuffer::inforBuffer;
    messageBuffer::inforBufferWriteLocker.release();;
    return ans;
}

QList<CardEntity*> messageBuffer::readCardID(int sum)
{
    QList<CardEntity*> cards;
    messageBuffer::cardBufferUsed.acquire(sum);
    for(int i = 0;i < sum;i++)
    {
        cards << getCardByID(messageBuffer::cardBuffer[messageBuffer::cardBufferBottom++]);
        if(messageBuffer::cardBufferBottom >= BUFFERSIZE)
            messageBuffer::cardBufferBottom = 0;
    }
    messageBuffer::cardBufferFree.release(sum);
    return cards;
}
QString messageBuffer::readMsg()
{
    messageBuffer::msgBufferReadLocker.acquire();;
    QString ans = messageBuffer::msg;
    messageBuffer::msgBufferWriteLocker.release();;
    return ans;
}
void messageBuffer::writeMsg(QString msg)
{
    messageBuffer::msgBufferWriteLocker.acquire();
    messageBuffer::msg = msg;
    messageBuffer::msgBufferReadLocker.release();
}
void messageBuffer::writeBatInfor(BatInfor content)
{
    messageBuffer::batBufferWriteLocker.acquire();;
    messageBuffer::batBuffer = content;
    messageBuffer::batBufferReadLocker.release();;
}

void messageBuffer::writeInfor(int content)
{
    messageBuffer::inforBufferWriteLocker.acquire();
    messageBuffer::inforBuffer = content;
    messageBuffer::inforBufferReadLocker.release();
}

void messageBuffer::writeCardInfor(QList<int> cardID)
{
    int sum = cardID.length();
    messageBuffer::cardBufferFree.acquire(sum);
    for(int i = 0;i < sum;i++)
    {
        messageBuffer::cardBuffer[messageBuffer::cardBufferTop++] = cardID.at(i);
        if(messageBuffer::cardBufferTop >= BUFFERSIZE)
            messageBuffer::cardBufferTop = 0;
    }
    messageBuffer::cardBufferUsed.release(sum);
}

ClientSocket::ClientSocket(QObject *parent) :
    QTcpSocket(parent)
{
    this->id = -1;
}
void ClientSocket::readMessage()
 {
    QDataStream in(this);
    quint16 nextBlockSize;
    QString message;

        in.setVersion(QDataStream::Qt_4_7);
        while(1)
        {
        if(bytesAvailable() < (int)sizeof(quint16)) return;
        in >> nextBlockSize;

        if(bytesAvailable() < nextBlockSize) return;

        //���û�еõ�ȫ�������ݣ��򷵻أ�������������

        in >> message;

        emit getMessage(id,message);
        }
        //�����յ������ݴ�ŵ�������
 }

Server::Server(QObject *parent,bool isIPv4,int port) :
    QTcpServer(parent)
{
    this->clientSocketList.clear();
    this->ready.clear();
    if(isIPv4)
        listen(QHostAddress::Any,port);
    else
        listen(QHostAddress::AnyIPv6,port);
}

void Server::incomingConnection ( int socketDescriptor )
{
    if(this->clientSocketList.size() >= playerSum)
        return;
    ClientSocket* newSocket=new ClientSocket;
    newSocket->setSocketDescriptor(socketDescriptor);
    /*
    for(int i = 0;i < this->clientSocketList.size();i++)
    {

        if(this->clientSocketList.at(i)->peerAddress() == newSocket->peerAddress())
        {
            if(this->clientSocketList.at(i)->peerPort() == newSocket->peerPort())
            {
                delete newSocket;
                return;
            }
        }
    }
    */
    clientSocketList.append(newSocket);
    ready.append(false);
    this->socketDescriptorList.append(socketDescriptor);
    newSocket->id=clientSocketList.count()-1;
    connect(newSocket,SIGNAL(disconnected()),newSocket,SLOT(onDisconnected()));
    connect(newSocket,SIGNAL(disconnectedSIN(ClientSocket*)),this,SLOT(onDisconnectedSIG(ClientSocket*)));
    connect(newSocket,SIGNAL(disconnected()),newSocket,SLOT(deleteLater()));
    connect(newSocket,SIGNAL(readyRead()),newSocket,SLOT(readMessage()));
    connect(newSocket,SIGNAL(getMessage(int,QString)),this,SLOT(decoder(int,QString)));

    QString temp = "1;";
    temp += QString::number(this->clientSocketList.size()-1);
    temp += ";";
    this->sendMessage(this->clientSocketList.size()-1,temp);

    if(this->clientSocketList.size() == playerSum)
        emit this->seatArrangeSIG();
}

//num=-1��ʾ�㲥
void Server::sendMessage(int num, QString message)
{
    QByteArray block; //�����ݴ�����Ҫ���͵�����

    QDataStream out(&block,QIODevice::WriteOnly);

    //ʹ��������д������

    out.setVersion(QDataStream::Qt_4_7);

    //�����������İ汾���ͻ��˺ͷ�������ʹ�õİ汾Ҫ��ͬ

    out<<(quint16) 0;

    out<<message;

    out.device()->seek(0);
    out<<(quint16)(block.size() - sizeof(quint16));


    QTcpSocket *clientConnection;
    if(num != -1)
    {
        clientConnection = clientSocketList.at(num);
        clientConnection->write(block);
    }
    else
    {
        for(int i = 0;i < this->clientSocketList.size();i++)
        {
            clientConnection = this->clientSocketList.at(i);
            clientConnection->write(block);
        }
    }
    emit this->toDisplay("send:");
    emit this->toDisplay(message);
    //qDebug()<<this->serverAddress().toString();

//    clientConnection->disconnectFromHost();

}

void Server::onDisconnectedSIG(ClientSocket* brokenSocket)
{
    clientSocketList.removeOne(brokenSocket);
    coder.notice("<font color=\'red\'>���"+QString::number(brokenSocket->getID())+"����</font>");
}

void Server::decoder(int id, QString message)
{
    QStringList infor;
    emit this->toDisplay("receive:");
    emit this->toDisplay(message);
    if(message == NULL)
        return;
    infor = message.split(";");

    QList<int> cards;
    QStringList chosenCards;
    BatInfor ans;
    BatInfor action;
    BatInfor reply;
    BatInfor missileInfor;
    int i,howMany;

    switch(infor.at(0).toInt())
    {
    case ACTIONCOMMAND:

        if(infor.at(1) == QString::number(ATTACK))
        {

            action.reply = infor.at(1).toInt();
            action.CardID = infor.at(2).toInt();
            if(infor.at(3) == "")
                action.dstID = 0;
            else
                action.dstID = infor.at(3).toInt();
            if(infor.at(4) == "")
                action.srcID = 0;
            else
                action.srcID = infor.at(4).toInt();

            messageBuffer::writeBatInfor(action);
        }
        else if(infor.at(1) == QString::number(SPECIAL))
        {
            action.reply = infor.at(1).toInt();
            action.CardID = infor.at(2).toInt();
            if(infor.at(3) == "")
                action.infor1 = 0;
            else
                action.infor1 = infor.at(3).toInt();
            if(infor.at(4) == "")
                action.infor2 = 0;
            else
                action.infor2 = infor.at(4).toInt();

            messageBuffer::writeBatInfor(action);
        }
        else if(infor.at(1).toInt() == MAGIC)
        {
            if(infor.at(2).toInt() == 0)
            {
                action.reply = MAGIC;
                action.infor1 = COMMONMAGIC;
                action.CardID = infor.at(3).toInt();
                action.dstID = infor.at(4).toInt();
                action.srcID = infor.at(5).toInt();

                messageBuffer::writeBatInfor(action);
            }
        }
        else if(infor.at(1).toInt() == FINISH)
        {
            action.reply = FINISH;
            messageBuffer::writeBatInfor(action);
        }
    break;

    case UNACTIONAL:
    action.reply = UNACTIONAL;
    messageBuffer::writeBatInfor(action);
    break;

    case REBATCOMMAND:

        reply.reply = infor.at(1).toInt();
        if(reply.reply != HIT)
        {
            reply.CardID = infor.at(2).toInt();
            reply.dstID = infor.at(3).toInt();
            reply.srcID = infor.at(4).toInt();
        }

        messageBuffer::writeBatInfor(reply);
    break;

    case DISCARDCOMMAND:

        chosenCards = infor.at(1).split(",");

        for(int i = 0;i < chosenCards.size();i++)
            cards << chosenCards.at(i).toInt();
        messageBuffer::writeCardInfor(cards);

    break;

    case READYBEGIN:

        ready[id] = true;
        for(int i = 0;i < ready.size();i++)
        {
            if(!ready.at(i))
                return;
        }
        emit characterNotice();
        emit this->gameStartSIG();
    break;

    case WEAKCOMMAND:

        messageBuffer::writeInfor(infor[1].toInt());
    break;

    case MISSILECOMMAND:

        missileInfor.reply = infor.at(1).toInt();
        if(missileInfor.reply != 2)
            missileInfor.CardID = infor.at(2).toInt();
        if(missileInfor.reply == 0)
            missileInfor.dstID = infor.at(3).toInt();

        messageBuffer::writeBatInfor(missileInfor);
    break;

    case ANSFORCROSS:
    messageBuffer::writeInfor(infor[1].toInt());
    break;

    case ANSFORSKILL:
    messageBuffer::writeInfor(infor[1].toInt());
    break;
//������
    case 101:
        action.reply=ATTACKSKILL;
        action.CardID = infor.at(1).toInt();
        action.dstID = infor.at(2).toInt();
        action.srcID = infor.at(3).toInt();
        action.infor1=101;
        messageBuffer::writeBatInfor(action);
        break;
//��Ӱ
    case 102:
        action.reply=ATTACKSKILL;
        action.CardID = infor.at(1).toInt();
        action.dstID = infor.at(2).toInt();
        action.srcID = infor.at(3).toInt();
        action.infor1 = 102;
        action.infor2= infor.at(4).toInt();
        messageBuffer::writeBatInfor(action);
        break;

    case BOWLADY_ANSFORTRANSFIXTION:

    ans.reply = infor[1].toInt();
    if(ans.reply != 0)
        ans.CardID = infor[2].toInt();
    messageBuffer::writeBatInfor(ans);
    break;

    case BOWLADY_TRAP:

    ans.reply = MAGIC;
    ans.infor1 = BOWLADY_TRAP;
    ans.CardID = infor[1].toInt();
    ans.dstID = infor[2].toInt();
    ans.srcID = infor[3].toInt();
    messageBuffer::writeBatInfor(ans);
    break;

    case BOWLADY_SNIPE:

    ans.reply = MAGIC;
    ans.infor1 = BOWLADY_SNIPE;
    ans.infor2 = infor[1].toInt();
    ans.dstID = infor[2].toInt();
    ans.srcID = infor[3].toInt();
    messageBuffer::writeBatInfor(ans);
    break;
//��ӡ����
    case 401:
        ans.reply = MAGIC;
        ans.infor1 = 401;
        ans.CardID = infor[1].toInt();
        ans.dstID = infor[2].toInt();
        ans.srcID = infor[3].toInt();
        messageBuffer::writeBatInfor(ans);
        break;
//��ϵ����
    case 402:
        ans.reply = MAGIC;
        ans.infor1 = 402;
        ans.infor2 = infor[1].toInt();
        ans.dstID = infor[2].toInt();
        ans.srcID = infor[3].toInt();
        messageBuffer::writeBatInfor(ans);
        break;
//��ӡ����
    case 403:
        ans.reply = MAGIC;
        ans.infor1 = 403;
        ans.infor2 = infor[1].toInt();
        ans.dstID = infor[2].toInt();
        ans.srcID=infor[3].toInt();
        ans.CardID = infor[4].toInt();
        messageBuffer::writeBatInfor(ans);
        break;
//��������
    case 404:
        action.reply=ATTACKSKILL;
        action.CardID = infor.at(1).toInt();
        action.dstID = infor.at(2).toInt();
        action.srcID = infor.at(3).toInt();
        action.infor1=404;
        messageBuffer::writeBatInfor(action);
        break;
//ˮӰ
    case 502:
        messageBuffer::writeMsg(message.remove(0,4));
        break;
//Ǳ��
    case 503:
        messageBuffer::writeInfor(infor[1].toInt());
        break;
//��˪����
    case 601:
        ans.dstID=infor[1].toInt();
        messageBuffer::writeBatInfor(ans);
        break;
//������
    case 602:
        ans.reply=MAGIC;
        ans.CardID=infor[1].toInt();
        ans.dstID=infor[2].toInt();
        ans.srcID=infor[3].toInt();
        ans.infor1=602;
        messageBuffer::writeBatInfor(ans);
        break;
//����֮��
    case 603:
        ans.reply=MAGIC;
        ans.CardID=infor[1].toInt();
        ans.srcID=infor[2].toInt();
        ans.infor1=603;
        int n;
        n=infor[3].toInt();
        ans.infor2=n;
        ans.dstID=infor[4].toInt();
        if (n>1)
            ans.infor3=infor[5].toInt();
        if (n>2)
            ans.infor4=infor[6].toInt();
        messageBuffer::writeBatInfor(ans);
        break;
//����
    case 604:
        messageBuffer::writeInfor(infor[1].toInt());
        break;
//ʥ��
    case 605:
        ans.reply=MAGIC;
        ans.infor4=infor[1].toInt();
        ans.dstID=infor[2].toInt();
        ans.infor1=605;
        ans.infor2=infor[3].toInt();
        ans.infor3=infor[4].toInt();
        ans.srcID=infor[5].toInt();
        messageBuffer::writeBatInfor(ans);
        break;
//��֮�ྻ
    case 701:
        ans.reply=MAGIC;
        ans.infor1=701;
        ans.CardID = infor[1].toInt();
        ans.dstID = infor[2].toInt();
        ans.srcID= infor[3].toInt();
        ans.infor2=infor[4].toInt();
        messageBuffer::writeBatInfor(ans);
        break;
//��ʹף��
    case 702:
        ans.reply = MAGIC;
        ans.infor1 = 702;
        ans.CardID = infor[1].toInt();
        ans.srcID= infor[2].toInt();
        ans.infor2= infor[3].toInt();
        ans.dstID = infor[4].toInt();
        if(ans.infor2==2)
            ans.infor3=infor[5].toInt();
        messageBuffer::writeBatInfor(ans);
        break;
//��ʹ֮ǽ
    case 703:
        ans.reply = MAGIC;
        ans.infor1 = 703;
        ans.CardID = infor[1].toInt();
        ans.dstID = infor[2].toInt();
        ans.srcID = infor[3].toInt();
        messageBuffer::writeBatInfor(ans);
        break;
//��ʹ֮��
    case 704:
        ans.reply=infor[1].toInt();
        if(ans.reply==1)
        {
            ans.infor1=infor[2].toInt();
            ans.dstID=infor[3].toInt();
            ans.CardID=infor[4].toInt();
        }
        messageBuffer::writeBatInfor(ans);
        break;
//��֮�ӻ�
    case 705:
        ans.reply=infor[1].toInt();
        if(ans.reply==1)
        {
            ans.infor1=infor[2].toInt();
            ans.infor2= infor[3].toInt();
        }
        messageBuffer::writeBatInfor(ans);
        break;
//��ʹ�
    case 707:
        ans.dstID=infor[1].toInt();
        messageBuffer::writeBatInfor(ans);
        break;
//��ʹף�������ƣ�
    case 751:
        howMany=infor[1].toInt();
        for(i=0;i<howMany;i++)
            cards<<infor[i+2].toInt();
        messageBuffer::writeCardInfor(cards);
        break;

//ħ���ں�(�غ��ڣ�
    case 801:
        ans.reply = MAGIC;
        ans.CardID = infor[1].toInt();
        ans.infor1 = 801;
        ans.dstID = infor[2].toInt();
        ans.srcID = infor[3].toInt();
        messageBuffer::writeBatInfor(ans);
        break;
//ħ���ںϣ��غ��⣩
    case 802:
        missileInfor.reply = 802;
        missileInfor.CardID = infor.at(1).toInt();
        missileInfor.dstID = infor.at(2).toInt();
        messageBuffer::writeBatInfor(missileInfor);
        break;
//ħ�����
    case 803:
        ans.reply = MAGIC;
        ans.infor1 = 803;
        ans.CardID = infor[1].toInt();
        ans.dstID = infor[2].toInt();
        ans.infor2 = infor[3].toInt();
        ans.srcID = infor[4].toInt();
        messageBuffer::writeBatInfor(ans);
        break;
//����籩
    case 804:
        ans.reply = MAGIC;
        ans.infor1 = 804;
        ans.dstID = infor[1].toInt();
        ans.infor2 = infor[2].toInt();
        ans.srcID = infor[3].toInt();
        messageBuffer::writeBatInfor(ans);
        break;
//ħ����������ƣ�
    case 851:
        ans.reply=infor[1].toInt();
        if(ans.reply)
        ans.CardID=infor[2].toInt();
        messageBuffer::writeBatInfor(ans);
        break;
//������ն
    case 901:
        action.reply=ATTACKSKILL;
        action.CardID = infor.at(1).toInt();
        action.dstID = infor.at(2).toInt();
        action.srcID = infor.at(3).toInt();
        action.infor1=901;
        messageBuffer::writeBatInfor(action);
        break;
//��Ӱ����
    case 902:
        ans.reply = MAGIC;
        ans.infor1 = 902;
        ans.CardID = infor[1].toInt();
        ans.infor2 = infor[2].toInt();
        ans.dstID = infor[3].toInt();
        ans.srcID = infor[4].toInt();
        messageBuffer::writeBatInfor(ans);
        break;
//��Ӱ����
    case 903:
        messageBuffer::writeInfor(infor[1].toInt());
        break;
//��ҫ
    case 1001:
        ans.reply = MAGIC;
        ans.infor1 = 1001;
        ans.CardID = infor[1].toInt();
        ans.srcID = infor[2].toInt();
        messageBuffer::writeBatInfor(ans);
        break;
//�ͽ�
    case 1002:
        ans.reply = MAGIC;
        ans.infor1 = 1002;
        ans.CardID = infor[1].toInt();
        ans.dstID = infor[2].toInt();
        ans.srcID = infor[3].toInt();
        messageBuffer::writeBatInfor(ans);
        break;
//��ǹ
    case 1005:
        messageBuffer::writeInfor(infor[1].toInt());
        break;
//ʥ������
    case 1006:
        ans.reply = MAGIC;
        ans.infor1 = 1006;
        ans.srcID = infor[1].toInt();
        messageBuffer::writeBatInfor(ans);
        break;
//��թ
    case 1201:
        action.reply=ATTACKSKILL;
        action.CardID = infor.at(1).toInt();
        action.dstID = infor.at(2).toInt();
        action.srcID = infor.at(3).toInt();
        action.infor1=1201;
        action.infor2=infor.at(4).toInt();
        action.infor3=infor.at(5).toInt();
        action.infor4=infor.at(6).toInt();
        if(action.infor2==3)
            action.infor5=infor.at(7).toInt();
        messageBuffer::writeBatInfor(action);
        break;
//͵�컻��
    case 1202:
        action.reply = MAGIC;
        action.infor1 = 1202;
        action.infor2 = infor[1].toInt();
        action.srcID = infor[2].toInt();
        messageBuffer::writeBatInfor(action);
        break;
//����ӹ�
    case 1203:
        action.reply = MAGIC;
        action.infor1 = 1203;
        action.infor2 = infor[1].toInt();
        action.srcID = infor[2].toInt();
        messageBuffer::writeBatInfor(action);
        break;
//ð��������
    case 1204:
        action.reply = SPECIAL;
        action.infor1 = 1204;
        action.dstID = infor[1].toInt();
        action.srcID = infor[2].toInt();
        action.infor2 = infor[3].toInt();
        action.infor3 = infor[4].toInt();
        messageBuffer::writeBatInfor(action);
        break;
    }
}

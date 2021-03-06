#include "FengYin.h"

FengYin::FengYin()
{
    makeConnection();
    connect(playerArea,SIGNAL(playerUnready()),this,SLOT(onUnready()));

    Button *fengYinFaShu,*wuXiShuFu,*fengYinPoSui;
    fengYinFaShu=new Button(3,tr("封印法术"));
    buttonArea->addButton(fengYinFaShu);
    connect(fengYinFaShu,SIGNAL(buttonSelected(int)),this,SLOT(FengYinFaShu()));

    wuXiShuFu=new Button(4,tr("五系束缚"));
    buttonArea->addButton(wuXiShuFu);
    connect(wuXiShuFu,SIGNAL(buttonSelected(int)),this,SLOT(WuXiShuFu()));

    fengYinPoSui=new Button(5,tr("封印破碎"));
    buttonArea->addButton(fengYinPoSui);
    connect(fengYinPoSui,SIGNAL(buttonSelected(int)),this,SLOT(FengYinPoSui()));
}

void FengYin::normal()
{
    Role::normal();
    Player* myself=dataInterface->getMyself();
//封印法术
    foreach(Card*ptr, dataInterface->getHandCards())
        if (ptr->getType()=="attack"&&ptr->getProperty()==tr("幻"))
            buttonArea->enable(3);
//五系束缚
    if(myself->getEnergy()>0)
        buttonArea->enable(4);
//封印破碎
    if(myself->getEnergy()>0)
        foreach(Player*ptr,dataInterface->getPlayerList())
            if(ptr->hasStatus()){
                buttonArea->enable(5);
                break;
            }
    unactionalCheck();
}

void FengYin::FengYinFaShu()
{
    state=401;
    handArea->reset();
    playerArea->reset();
    tipArea->reset();

    playerArea->setQuota(1);
    handArea->setQuota(1);

    decisionArea->enable(1);
    decisionArea->disable(0);

    handArea->enableProperty(tr("幻"));
    handArea->disableMagic();
}

void FengYin::WuXiShuFu()
{
    state=402;
    int gem,crystal;
    Player*myself=dataInterface->getMyself();

    gem=myself->getGem();
    crystal=myself->getCrystal();

    handArea->reset();
    playerArea->reset();
    tipArea->reset();

    decisionArea->enable(1);
    decisionArea->disable(0);
    playerArea->setQuota(1);
    playerArea->enableEnemy();

    tipArea->setMsg(tr("请选择使用的能量："));
    if(crystal>=1)
        tipArea->addBoxItem(tr("1.水晶"));
    if(gem>=1)
        tipArea->addBoxItem(tr("2.宝石"));

    tipArea->showBox();
}

void FengYin::FengYinPoSui()
{
    state=403;
    int gem,crystal;
    Player*myself=dataInterface->getMyself();
    QList<Player*>players=dataInterface->getPlayerList();

    gem=myself->getGem();
    crystal=myself->getCrystal();

    handArea->reset();
    playerArea->reset();
    tipArea->reset();

    decisionArea->enable(1);
    decisionArea->disable(0);
    playerArea->setQuota(1);
    for(int i=0;i<players.size();i++)
        if(players[i]->hasStatus())
            playerArea->enablePlayerItem(i);

    tipArea->setMsg(tr("请选择使用的能量："));
    if(crystal>=1)
        tipArea->addBoxItem(tr("1.水晶"));
    if(gem>=1)
        tipArea->addBoxItem(tr("2.宝石"));

    tipArea->showBox();
}

void FengYin::onUnready()
{
    switch(state)
    {
    case 403:
        tipArea->reset();
        FengYinPoSui();
        break;
    }
}

void FengYin::cardAnalyse()
{
    Role::cardAnalyse();

    switch (state)
    {
    case 401:
        QList<Player*>players=dataInterface->getPlayerList();
        QList<Card*>selectedCards=handArea->getSelectedCards();
        playerArea->enableEnemy();
        for(int i=0;i<players.size();i++)
            for(int j=0;j<players[i]->getStatus().size();j++)
                if(players[i]->checkStatus(selectedCards[0]->getSpecialityList().at(0))){
                    playerArea->disablePlayerItem(i);
                    break;
                }
        decisionArea->enable(1);
        break;
    }
}
void FengYin::playerAnalyse()
{
    QString text;
    switch (state)
    {
    case 403:
        text=tipArea->getBoxCurrentText();
        if(text[0].digitValue()==1)
            usedGem=0;
        else
            usedGem=1;
        tipArea->showStatus(playerArea->getSelectedPlayers().at(0)->getID());
        break;
    default:
        decisionArea->enable(0);
        break;
    }
}

void FengYin::onOkClicked()
{
    if(state==10&&flag==1)
        state=404;
    Role::onOkClicked();
    QList<Card*>selectedCards;
    QList<Player*>selectedPlayers;

    QString command;
    QString cardID;
    QString sourceID;
    QString targetID;
    QString text;

    selectedCards=handArea->getSelectedCards();
    selectedPlayers=playerArea->getSelectedPlayers();

    switch(state)
    {
//额外行动询问
    case 42:
        text=tipArea->getBoxCurrentText();
        flag=text[0].digitValue();
        if(flag){
            attackAction();
        }
        break;
//封印法术
    case 401:
        command="401;";
        cardID=QString::number(selectedCards[0]->getID());
        sourceID=QString::number(myID);
        targetID=QString::number(selectedPlayers[0]->getID());
        command+=cardID+";"+targetID+";"+sourceID+";";
        dataInterface->removeHandCard(selectedCards[0]);
        usedMagic=true;
        emit sendCommand(command);
        gui->reset();
        break;
//五系束缚
    case 402:
        command="402;";
        text=tipArea->getBoxCurrentText();
        if(text[0]=='1')
            command+="0;";
        else
            command+="1;";
        sourceID=QString::number(myID);
        targetID=QString::number(selectedPlayers[0]->getID());
        command+=targetID+";"+sourceID+";";
        usedMagic=true;
        emit sendCommand(command);
        gui->reset();
        break;
//封印破碎
    case 403:
        command="403;";
        if(usedGem)
            command+="1;";
        else
            command+="0;";
        command+=QString::number(selectedPlayers[0]->getID())+";";
        command+=QString::number(myID)+";";
        command+=QString::number(tipArea->getSelectedCardID())+";";
        usedMagic=true;
        emit sendCommand(command);
        gui->reset();
        break;
//法术激荡
    case 404:
        cardID=QString::number(selectedCards[0]->getID());
        targetID=QString::number(selectedPlayers[0]->getID());
        sourceID=QString::number(myID);
        command="404;"+cardID+";"+targetID+";"+sourceID+";";
        dataInterface->removeHandCard(selectedCards[0]);
        gui->reset();
        usedAttack=true;
        usedMagic=usedSpecial=false;
        actions.removeOne(tr("1.法术激荡"));
        emit sendCommand(command);
        break;
    }
}


void FengYin::onCancelClicked()
{
    Role::onCancelClicked();
    switch(state)
    {
//特殊行动
    case 1:
//封印法术
    case 401:
//五系束缚
    case 402:
//封印破碎
    case 403:
        normal();
        break;
    }
}

void FengYin::decipher(QString command)
{
    Role::decipher(command);
    QStringList arg=command.split(';');
    int targetID;
    QString flag;

    switch (arg[0].toInt())
    {
//行动阶段 flag 0-所有行动，1-攻击行动，2-法术行动，3-特殊行动，4-攻击或法术行动
    case 29:
        targetID=arg[1].toInt();
        flag=arg[2];
        if(targetID==myID)
        {
            if(flag=="0")
                normal();
        }
        break;
//额外行动询问
    case 42:
        targetID=arg[1].toInt();
        if(targetID==myID)
        {
            if(usedMagic)
                actions.append(tr("1.法术激荡"));

            foreach(QString ptr,actions)
                tipArea->addBoxItem(ptr);
            tipArea->showBox();

            state=42;
        }
        break;
    }
}

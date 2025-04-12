#include <stdio.h> 
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

// (a)属性
typedef enum Element {FLAME, AQUA, LEAF, GROUND, SOUL, EMPTY} Element;

// (b)属性別の記号
const char ELEMENT_MARK[EMPTY+1] = {'*','~','%','#','+', ' '};

// (d) 宝石の最大個数を定義する
enum {MAX_GEMS = 14, BLUR_DAMAGE = 10, RECOVER_NUM = 20};

// (e) 攻撃元の属性と攻撃を受ける属性を添字として入れるとダメージ増幅率が取り出せる二次元配列
const double BOOST_DAMAGE[GROUND + 1][GROUND + 1] = {
    //FLAME AQUA LEAF GROUND
    {1.0, 0.5, 2.0, 1.0},
    {2.0, 1.0, 1.0, 0.5},
    {0.5, 1.0, 1.0, 2.0},
    {1.0, 2.0, 0.5, 1.0},
};

// (f) モンスター
typedef struct MONSTER {
  const char* name;
  Element element;
  const int maxhp;
  int hp;
  const int attack;
  const int defense;
} Monster;

// (g)ダンジョン
typedef struct DUNGEON {
  Monster* monsters;
  const int numMonsters;
} Dungeon;

// (h)パーティ関連情報
typedef struct PARTY {
  const char* playername;
  Monster* partyMonsters;
  const int numMonsters;
  int partyHp;
  const int aveDeffence;
  const int maxPartyHp;
} Party;

// (i) バトルフィールド構造体
typedef struct BATTLEFIELD {
    Party* pParty;
    Monster* pMonsters;
    Element gems[MAX_GEMS];
} BattleField;

// (j) 消去可能な宝石の並びに関する構造体
typedef struct BANISHINFO {
  Element banishableElement;
  int banishableFrom;
  int banishableNum;
} Banishinfo;

// Prototype Declations

int traverseDungeon(char* playerName, Dungeon* pDungeon, Party* pPartyinfo);
int engageCombat(Party* pPartyinfo, Monster* pEnemy);
Party assembleTeam(char* playerName,Monster* pMonsters, int monsterNum);
void displayTeam(Party* pPartyinfo);
void playerTurn(BattleField* pBattleField);
void performAttack(BattleField* pField, Banishinfo* bi, int comboNum, int allBanishnum);
void enemyTurn(BattleField* pBattleField);
void enemyStrike(Monster* pEnemy, Party* pPartyinfo);
void displayBattlefield(BattleField* pBattleField);
bool validateCommand(char* pInput);
void exchangeGem(Element* gems, int pos, int step);
void checkAllGems(BattleField* pField);
Banishinfo identifyRemovableGems(Element* pGems);
void removeGems(BattleField* pField, Banishinfo* banishable, int comboNum, int allBanishNum);
void compactGems(Element* pGems);
void recoverHealth(BattleField* pField, int comboNum, int allBanishNum);

// ユーティリティ関数
void showMonsterName(Monster* monster);
void initializeGems(BattleField* pBattleField);
void displayGems(Element* pGems);
void displayGem(Element e);
void shiftGemPosition(Element* pGems, int fromPos, int toPos, bool printProcess);
int countSpecificGmes(Element* pGems, Element target);
void generateNewGems(Element* pGems);
int randomizedDamage(int base, int blurNum);
int computeEnemyAttack(Monster* pEnemy, Party* pParty);
int computePartyAttack(Monster* pEnemy, Monster* pAttacker, int comboNum, int allBanishNum);
int computeRecoveryAmount(Party* pParty, int comboNum, int allBanishNum);

// Functions Declation parts

// (1)ゲーム開始から終了までの流れ
int main(int argc, char** argv)
{
  srand((unsigned)time(NULL));

  char playerName[] = "test";

  printf("*** Monster Battle ***\n");

  // ダンジョンの準備
  Monster dungeonMonsters[] = {
    {"Slime", AQUA, 100, 100, 10,  5},
    {"Goblin", GROUND, 200, 200, 20, 15},
    {"Crow", LEAF, 300, 300, 30, 25},
    {"Bear", LEAF, 400, 400, 40, 30},
    {"Devil", FLAME,  800, 800, 50, 40}
  };
  Dungeon dungeon = {dungeonMonsters, 5};

    // 味方パーティの準備
  Monster myMonsters[] = {
    {"Efreet", FLAME, 150, 150, 25,  10},
    {"Wing", LEAF, 150, 150, 15, 10},
    {"Tiger", GROUND, 150, 150, 20, 5},
    {"Leviathan", AQUA, 150, 150, 20, 15},
  };

  //(4) assembleTeam関数
   Party partyInfo = assembleTeam(playerName, myMonsters, 4); 

  // いざ、ダンジョンへ
  int winCount = traverseDungeon(playerName, &dungeon, &partyInfo);

  // 冒険終了後
  if(winCount == dungeon.numMonsters) {
    printf("***GAME CLEAR!***\n");
  }  else {
    printf("***GAME OVER***\n");
  }
  printf("倒したモンスター数＝%d\n", winCount);
  return 0;
}

// (2)ダンジョン開始から終了までの流れ
int traverseDungeon(char* playerName, Dungeon* pDungeon, Party* pPartyinfo)
{
  printf("%sのパーティ(HP=%d)はダンジョンに到着した\n", playerName, pPartyinfo->partyHp);
    //(5) パーティ情報を表示
    displayTeam(pPartyinfo);    

  // そのダンジョンでバトルを繰り返す
  int winCount = 0;
  for(int i = 0; i < pDungeon->numMonsters; i++) {
    winCount += engageCombat(pPartyinfo, &(pDungeon->monsters[i]));
  }

  printf("%sのパーティはダンジョンを制覇した！\n", playerName, pPartyinfo->partyHp);
  return winCount;
}

// (3)バトル開始から終了までの流れ
int engageCombat(Party* pPartyinfo, Monster* pEnemy)
{
    BattleField battleField = {pPartyinfo, pEnemy};
    initializeGems(&battleField);

  showMonsterName(pEnemy);
  printf("が現れた！\n\n");

  while (pPartyinfo->partyHp > 0 && pEnemy->hp > 0){
    playerTurn(&battleField);
    if (pEnemy->hp > 0){
        enemyTurn(&battleField);
    }
  }

  // ダミーのため速攻倒す
  int count;
  if (pEnemy->hp <= 0){
    showMonsterName(pEnemy);
    printf("を倒した！\n\n");
    printf("%sはさらに奥へと進んだ\n\n", pPartyinfo->playername);
    count = 1;
  } else if (pPartyinfo->partyHp <= 0){
    printf("敵モンスターに敗れた！\n");
    count = 0;
  }
  return count;
}

// (4)味方モンスターとプレイヤー名を受け取ってパーティ情報構造体を返却
Party assembleTeam(char* playerName,Monster* pMonsters, int monsterNum)
{
    int partyHp = 0;
    int sumDefence = 0;
    int aveDefence = 0;
   for (int i=0; i<monsterNum; i++){
    partyHp += pMonsters[i].hp; 
    sumDefence += pMonsters[i].defense;
   };
   aveDefence = sumDefence / monsterNum;
   Party p = {playerName, pMonsters, monsterNum, partyHp, aveDefence, partyHp}; 
   return p;
}

//(5) パーティ情報の表示
void displayTeam(Party* pPartyinfo)
{
    printf("パーティ編成------\n");
    for (int i=0;i<pPartyinfo->numMonsters;i++){
        showMonsterName(&(pPartyinfo->partyMonsters)[i]);
        printf(" HP=%d Attack=%d Defence=%d\n",
            (pPartyinfo->partyMonsters)[i].hp,
            (pPartyinfo->partyMonsters)[i].attack,
            (pPartyinfo->partyMonsters)[i].defense
        );
    }
    printf("-------------\n");
}

//(6) onPlayerTunrn関数
void playerTurn(BattleField* pBattleField)
{
    printf("【%sのターン】\n", pBattleField->pParty->playername);
    displayBattlefield(pBattleField);
    char userInput[3];
    do {
        printf("コマンド?>");
        scanf("%2s", userInput);
    } while(validateCommand(userInput) == false);
    shiftGemPosition(pBattleField->gems, userInput[0] - 'A', userInput[1]- 'A', true);
    checkAllGems(pBattleField);
}

//(7) performAttack関数
void performAttack(BattleField* pField, Banishinfo* bi, int comboNum, int allBanishnum)
{
    for (int i = 0; i < pField->pParty->numMonsters; i++){
        Monster attacker = pField->pParty->partyMonsters[i];
        
        if (attacker.element == bi->banishableElement){
            int damage = computePartyAttack(pField->pMonsters, &attacker, comboNum, allBanishnum);
            pField->pMonsters->hp -= damage;
            
            if (comboNum == 1){
                showMonsterName(&attacker);
                printf("の攻撃 !\n");
            } else {
                showMonsterName(&attacker);
                printf("の攻撃 ! %d COMBO!\n", comboNum);
            }
            showMonsterName(pField->pMonsters);
            printf("に %d のダメージを与えた\n\n", damage);
        }
    }
}

//(8) enemyTurn関数実装
void enemyTurn(BattleField* pField)
{
    printf("【");
    showMonsterName(pField->pMonsters);
    printf("のターン】\n");
    enemyStrike(pField->pMonsters, pField->pParty);
}

//(9) enemyStrike関数の実装
void enemyStrike(Monster* pEnemy, Party* pPartyinfo)
{
    int damage = computeEnemyAttack(pEnemy, pPartyinfo);
    pPartyinfo->partyHp -= damage; 
    printf("%d のダメージを受けた\n\n", damage);
}

//(10) displayBattlefield関数
void displayBattlefield(BattleField* pBattleField)
{
    printf("-------------------------\n");
    showMonsterName(pBattleField->pMonsters);
    printf(" HP= %d / %d \n\n\n", pBattleField->pMonsters->hp, pBattleField->pMonsters->maxhp);
    for (int i = 0 ; i < pBattleField->pParty->numMonsters; i++){
        showMonsterName(&pBattleField->pParty->partyMonsters[i]);
        printf(" ");
    }
    printf("\n");
    printf(" HP= %d / %d\n", pBattleField->pParty->partyHp, pBattleField->pParty->maxPartyHp);
    printf("-------------------------\n");
    printf(" ");
    for (int i = 0; i < MAX_GEMS; i++){
      printf("%c ", 'A'+i);
    }
    printf("\n");
    displayGems(pBattleField->gems);
    printf("-------------------------\n");
}

// (11) ユーザー入力をチェックする関数
bool validateCommand(char* pInput)
{
    if(strlen(pInput) != 2) return false;
    if (pInput[0] < 'A'|| pInput[0] > 'N') return false;
    if (pInput[1] < 'A'|| pInput[1] > 'N') return false;
    if (pInput[0] == pInput[1]) return false;
    return true;
}

// (12)宝石の並びに合わせて処理する関数
void checkAllGems(BattleField* pField)
{
  Banishinfo banishable = identifyRemovableGems(pField->gems);
  int comboNum = 1;
  int allBanishNum = banishable.banishableNum;

  if(banishable.banishableNum != 0) {
    removeGems(pField, &banishable, comboNum, allBanishNum);
    compactGems(pField->gems);

    while (true) {
        Banishinfo combo = identifyRemovableGems(pField->gems);
        if(combo.banishableNum != 0) {
            comboNum++;
            allBanishNum = combo.banishableNum;

            removeGems(pField, &combo, comboNum, allBanishNum);
            compactGems(pField->gems);
        } else if(combo.banishableNum == 0) break;
    }

    generateNewGems(pField->gems);
    
    while (true) {
        Banishinfo combo = identifyRemovableGems(pField->gems);
        if(combo.banishableNum != 0) {
            comboNum++;
            allBanishNum = combo.banishableNum;

            removeGems(pField, &combo, comboNum, allBanishNum);
            compactGems(pField->gems);
        } else if(combo.banishableNum == 0) break;
    }
    
    generateNewGems(pField->gems);
  }
}

// (13) 消去可能な宝石の並びがないか確認する関数
Banishinfo identifyRemovableGems(Element* pGems)
{
  const int threshold = 3;
  for(int i = 0; i < MAX_GEMS - threshold + 1; i++) {
    int banishNum = 1;
    if (pGems[i] == EMPTY) continue;
    for(int j = i + 1; j < MAX_GEMS; j++) {
      if(pGems[i] == pGems[j]) {
        banishNum++;
      } else {
        break;
      }
    }
    if(banishNum >= threshold) {
      Banishinfo bi = {pGems[i], i, banishNum};
    return bi;
    }
  }
  Banishinfo notBanish = {EMPTY, 0, 0};
  return notBanish;
}

// (14) 宝石を消去して効果を発動する関数
void removeGems(BattleField* pField, Banishinfo* banishable, int comboNum, int allBanishNum)
{
  for (int i = banishable->banishableFrom; i < banishable->banishableFrom + banishable->banishableNum; i++) {
    pField->gems[i] = EMPTY;
  }
    displayGems(pField->gems);
    switch(banishable->banishableElement){
        case FLAME: case LEAF: case AQUA: case GROUND:
            performAttack(pField, banishable, comboNum, allBanishNum);
            break;
        case SOUL:
            recoverHealth(pField,  comboNum, allBanishNum);
    }
}

// (15) 宝石が消去された後、残ったGemを左詰する関数
void compactGems(Element* pGems)
{
  int banishNum = countSpecificGmes(pGems, EMPTY);
  for(int i = 0; i < MAX_GEMS - banishNum; i++) {
    if(pGems[i] == EMPTY) {
      shiftGemPosition(pGems, i, MAX_GEMS - 1, false);
      i--;
    }
  }
  displayGems(pGems);
}

// (16) 消滅したGemスロットに再度Gemを充填する関数
void generateNewGems(Element* pGems)
{
  for(int i = 0; i < MAX_GEMS; i++) {
    if(pGems[i] == EMPTY) {
      pGems[i] = rand()%EMPTY;
    }
  }
  displayGems(pGems);
  printf("\n");
}

void recoverHealth(BattleField* pField, int comboNum, int allBanishNum)
{
    int damage = computeRecoveryAmount(pField->pParty, comboNum, allBanishNum);
    pField->pParty->partyHp += damage;
    printf("HPが %d 回復した!\n", damage);
}

// (A)モンスター名のカラー表示
void showMonsterName(Monster* pMonster)
{
  char symbol = ELEMENT_MARK[pMonster->element];

  printf("%c%s%c", symbol, pMonster->name, symbol);
}

// (B) initializeGems関数
void initializeGems(BattleField* pBattleField)
{
    for (int i=0 ; i<MAX_GEMS; i++){
        pBattleField->gems[i] = rand()%EMPTY;
    };
}

// (C) displayGems関数
void displayGems(Element* pGems)
{
    printf(" ");
    for (int i = 0; i < MAX_GEMS; i++){
        displayGem(pGems[i]);
    }
    printf("\n");
}

// (D) displayGem関数
void displayGem(Element e)
{
    printf("%c ", ELEMENT_MARK[e]);
}

// (E) shiftGemPosition関数
void shiftGemPosition(Element* pGems, int fromPos, int toPos, bool printProcess)
{
    int step = (toPos > fromPos) ? 1 : -1;

    displayGems(pGems);
    for(int i = fromPos; i != toPos; i += step){
        exchangeGem(pGems, i, step);
        if(printProcess) displayGems(pGems);
    }
}

// (F) Gemを一つずつ動かす関数
void exchangeGem(Element* gems, int pos, int step)
{
    Element buf = gems[pos];
    gems[pos] = gems[pos + step];
    gems[pos + step] = buf;
}

// (G) 特定のGemをカウントする関数
int countSpecificGmes(Element* pGems, Element target)
{
  int count = 0;
  for(int i=0; i < MAX_GEMS; i++) {
    if(pGems[i] == target) {
      count++;
    }
  }
  return count;
}

int randomizedDamage(int base, int blurNum)
{
    int blurPercent = rand() % (blurNum * 2 + 1) - blurNum + 100;
    int damage = base * blurPercent / 100;
    return damage;
}

int computeEnemyAttack(Monster* pEnemy, Party* pParty)
{
    int base = pEnemy->attack - pParty->aveDeffence;
    int damage = randomizedDamage(base, BLUR_DAMAGE);
    if (damage <= 0) damage = 1;
    return damage;
}

int computePartyAttack(Monster* pEnemy, Monster* pAttacker, int comboNum, int allBanishNum)
{
    int base = (pAttacker->attack - pEnemy->defense) *
    BOOST_DAMAGE[pAttacker->element][pEnemy->element];
    double comboPlus = 1;

    if (comboNum >= 2){
        for (int i = 0; i < allBanishNum -3 + comboNum; i++){
            comboPlus *= 1.5;
        }
    }

    int damage = randomizedDamage(base * comboPlus, BLUR_DAMAGE);
    return damage;
}

int computeRecoveryAmount(Party* pParty, int comboNum, int allBanishNum)
{
    int hpBuf = pParty->partyHp;
    double comboPlus = 1;

    if (comboNum >= 2){
        for (int i = 0; i < allBanishNum -3 + comboNum; i++){
            comboPlus *= 1.5;
        }
        printf("allBanishNum: %d, comboNum: %d, comboPlus: %f\n\n", 
        allBanishNum, comboNum, comboPlus);
    }

    int recover = randomizedDamage(RECOVER_NUM * comboPlus, BLUR_DAMAGE);

    if(pParty->partyHp >= pParty->maxPartyHp){
        recover = pParty->maxPartyHp -hpBuf;
    };

    return recover;
}
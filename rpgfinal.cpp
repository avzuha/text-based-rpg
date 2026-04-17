#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <tuple>

using namespace std;
int randInt(int lo, int hi) { return lo + rand() % (hi - lo + 1); }

int myMax(int a, int b) { return a > b ? a : b; }
int myMin(int a, int b) { return a < b ? a : b; }

void printLine(char c = '-', int len = 50) { cout << string(len, c) << "\n"; }

void pause() {
    cout << "\n[Press ENTER to continue...]";
    cin.ignore(1000, '\n');
}
struct Item {
    string name;
    string type;  
    int    value;  

    Item(const string& n, const string& t, int v)
        : name(n), type(t), value(v) {}
};
class Character {
protected:
    string name;
    int    hp, maxHp;
    int    attack, defense;
    int    level, xp, xpToNext;
    vector<Item> inventory;
    int    equippedWeaponBonus = 0;
    int    equippedArmorBonus  = 0;
public:
    Character(const string& n, int hp, int atk, int def)
        : name(n), hp(hp), maxHp(hp), attack(atk), defense(def),
          level(1), xp(0), xpToNext(100) {}
    virtual ~Character() = default;
    string getName()    const { return name; }
    int    getHp()      const { return hp; }
    int    getMaxHp()   const { return maxHp; }
    int    getLevel()   const { return level; }
    int    getXp()      const { return xp; }
    int    getXpNext()  const { return xpToNext; }
    bool   isAlive()    const { return hp > 0; }
    virtual int getDamage() const {
        return myMax(1, attack + equippedWeaponBonus + randInt(-2, 2));
    }
    void takeDamage(int dmg) {
        int reduced = myMax(1, dmg - (defense + equippedArmorBonus));
        hp = myMax(0, hp - reduced);
        cout << name << " takes " << reduced << " damage! (HP: " << hp << "/" << maxHp << ")\n";
    }
    void addItem(const Item& item) {
        inventory.push_back(item);
        cout << "  [+] Added \"" << item.name << "\" to inventory.\n";
    }
    void showInventory() const {
        printLine();
        cout << name << "'s Inventory:\n";
        if (inventory.empty()) { cout << "  (empty)\n"; return; }
        for (size_t i = 0; i < inventory.size(); ++i) {
            cout << "  " << i + 1 << ". " << inventory[i].name;
            for (int s = inventory[i].name.size(); s < 20; ++s) cout << ' ';
            cout << " [" << inventory[i].type << ", val:" << inventory[i].value << "]\n";
        }
    }
    bool useItem(int idx) {
        if (idx < 0 || idx >= (int)inventory.size()) {
            cout << "Invalid item.\n"; return false;
        }
        Item& it = inventory[idx];
        if (it.type == "potion") {
            int heal = myMin(it.value, maxHp - hp);
            hp += heal;
            cout << "  Used " << it.name << " — healed " << heal
                 << " HP! (HP: " << hp << "/" << maxHp << ")\n";
            inventory.erase(inventory.begin() + idx);
            return true;
        } else if (it.type == "weapon") {
            equippedWeaponBonus = it.value;
            cout << "  Equipped " << it.name << " (+" << it.value << " ATK bonus).\n";
            return true;
        } else if (it.type == "armor") {
            equippedArmorBonus = it.value;
            cout << "  Equipped " << it.name << " (++" << it.value << " DEF bonus).\n";
            return true;
        }
        return false;
    }
    void gainXp(int amount) {
        xp += amount;
        cout << "  [XP] +" << amount << " XP  (" << xp << "/" << xpToNext << ")\n";
        while (xp >= xpToNext) levelUp();
    }
    virtual void levelUp() {
        xp -= xpToNext;
        ++level;
        xpToNext = static_cast<int>(xpToNext * 1.5);
        maxHp   += 15;
        hp       = maxHp;
        attack  += 3;
        defense += 2;
        printLine('*');
        cout << "  ★  LEVEL UP!  " << name << " is now Level " << level << "!\n";
        cout << "     HP +" << 15 << "  ATK +" << 3 << "  DEF +" << 2 << "\n";
        printLine('*');
    }
    virtual void showStatus() const {
        printLine();
        cout << "  Name   : " << name    << "  (Lv." << level << ")\n"
             << "  HP     : " << hp      << " / " << maxHp << "\n"
             << "  Attack : " << attack  << " (+" << equippedWeaponBonus << " equipped)\n"
             << "  Defense: " << defense << " (+" << equippedArmorBonus  << " equipped)\n"
             << "  XP     : " << xp      << " / " << xpToNext << "\n";
    }
    virtual void specialAttack(Character& target) = 0;
    virtual string getClass() const = 0;
};
class Warrior : public Character {
    int rageCharges;
public:
    Warrior(const string& n)
        : Character(n, 120, 18, 10), rageCharges(3) {}
    string getClass() const override { return "Warrior"; }
    void showStatus() const override {
        Character::showStatus();
        cout << "  Rage   : " << rageCharges << " charges\n";
    }
    void specialAttack(Character& target) override {
        if (rageCharges <= 0) {
            cout << "  No Rage charges left!\n"; return;
        }
        --rageCharges;
        int dmg = static_cast<int>(getDamage() * 1.8);
        cout << "  WARRIOR  " << name << " unleashes POWER STRIKE!\n";
        target.takeDamage(dmg + 4);
    }
    void levelUp() override {
        Character::levelUp();
        rageCharges += 1;
        cout << "     Rage charges now: " << rageCharges << "\n";
    }
};
class Mage : public Character {
    int mana, maxMana;
public:
    Mage(const string& n)
        : Character(n, 80, 22, 5), mana(60), maxMana(60) {}
    string getClass() const override { return "Mage"; }
    int getDamage() const override {
        return myMax(1, attack + equippedWeaponBonus + randInt(-4, 6));
    }
    void showStatus() const override {
        Character::showStatus();
        cout << "  Mana   : " << mana << " / " << maxMana << "\n";
    }
    void specialAttack(Character& target) override {
        if (mana < 20) {
            cout << "  Not enough mana!\n"; return;
        }
        mana -= 20;
        int dmg = getDamage() * 2;
        cout << "  MAGE  " << name << " casts FIREBALL!\n";
        target.takeDamage(dmg);
    }

    void levelUp() override {
        Character::levelUp();
        maxMana += 20;
        mana     = maxMana;
        cout << "     Mana cap now: " << maxMana << "\n";
    }
};
class Enemy : public Character {
    string type;
    int    xpReward;
    Item*  lootItem; 
public:
    Enemy(const string& n, const string& t, int hp, int atk, int def,
          int xpRew, Item* loot = nullptr)
        : Character(n, hp, atk, def), type(t), xpReward(xpRew), lootItem(loot) {}
    ~Enemy() override { delete lootItem; }
    string getClass()  const override { return "Enemy[" + type + "]"; }
    int    getXpReward() const        { return xpReward; }
    Item*  getLoot()   const          { return lootItem; }
    void specialAttack(Character& target) override {
        cout << "  ENEMY  " << name << " uses HEAVY BLOW!\n";
        target.takeDamage(static_cast<int>(getDamage() * 1.5));
    }
    void showStatus() const override {
        cout << "  [Enemy] " << name << " (" << type << ")"
             << "  HP: " << hp << "/" << maxHp
             << "  ATK: " << attack << "  DEF: " << defense << "\n";
    }
};
Enemy* spawnEnemy(int playerLevel) {
    int tier = myMin(playerLevel, 5);
    vector<tuple<string,string,int,int,int,int,Item*>> pool = {
        {"Goblin",       "Beast",  30+tier*5,  6+tier,  2+tier/2, 30+tier*10,
         randInt(1,3)==1 ? new Item("Short Sword","weapon",3) : nullptr},
        {"Skeleton",     "Undead", 40+tier*6,  8+tier,  4+tier,   40+tier*12,
         randInt(1,3)==1 ? new Item("Bone Shield","armor",3) : nullptr},
        {"Dark Mage",    "Mage",   35+tier*5, 12+tier,  2,        50+tier*15,
         randInt(1,2)==1 ? new Item("Mana Potion","potion",30) : nullptr},
        {"Troll",        "Beast",  70+tier*8,  9+tier,  6+tier,   60+tier*18,
         randInt(1,4)==1 ? new Item("Health Potion","potion",50) : nullptr},
        {"Dragon Whelp", "Dragon", 90+tier*10,14+tier,  8+tier,   80+tier*22,
         new Item("Dragon Scale","armor",7)},
    };
    auto& [n,t,h,a,d,x,l] = pool[randInt(0, (int)pool.size()-1)];
    return new Enemy(n, t, h, a, d, x, l);
}
void battle(Character& hero, Enemy& enemy) {
    printLine('=');
    cout << "     BATTLE: " << hero.getName() << " vs " << enemy.getName() << "\n";
    printLine('=');
    while (hero.isAlive() && enemy.isAlive()) {
        hero.showStatus();
        enemy.showStatus();
        printLine();
        cout << "  Choose action:\n"
             << "  1. Normal Attack\n"
             << "  2. Special Attack\n"
             << "  3. Use Item\n"
             << "  4. Flee\n"
             << "  > ";
        int choice;
        cin >> choice;
        switch (choice) {
        case 1: {
            int dmg = hero.getDamage();
            cout << "  " << hero.getName() << " attacks for "
                 << dmg << " damage!\n";
            enemy.takeDamage(dmg);
            break;
        }
        case 2:
            hero.specialAttack(enemy);
            break;
        case 3: {
            hero.showInventory();
            if (hero.getHp() > 0) {
                cout << "  Enter item number (0 to cancel): ";
                int idx; cin >> idx;
                if (idx > 0) hero.useItem(idx - 1);
            }
            break;
        }
        case 4:
            if (randInt(1,3) == 1) {
                cout << "  Failed to flee!\n";
            } else {
                cout << "  You fled from battle!\n";
                return;
            }
            break;
        default:
            cout << "  Invalid — skipping turn.\n";
        }
        if (!enemy.isAlive()) break;
        cout << "\n  -- " << enemy.getName() << "'s turn --\n";
        if (randInt(1, 4) == 1) {          
            enemy.specialAttack(hero);
        } else {
            int dmg = enemy.getDamage();
            cout << "  " << enemy.getName() << " attacks!\n";
            hero.takeDamage(dmg);
        }
        pause();
    }
    printLine('=');
    if (hero.isAlive()) {
        cout << "     Victory!  " << hero.getName() << " defeated " << enemy.getName() << "!\n";
        hero.gainXp(enemy.getXpReward());
        if (enemy.getLoot()) {
            cout << "    Loot dropped: " << enemy.getLoot()->name << "!\n";
            hero.addItem(*enemy.getLoot());
        }
    } else {
        cout << "  PLAYER  " << hero.getName() << " was defeated...\n";
    }
    printLine('=');
    pause();
}
void randomEvent(Character& hero) {
    int roll = randInt(1, 6);
    printLine('~');
    cout << "    RANDOM EVENT!\n";
    switch (roll) {
    case 1:
        cout << "You find a glowing Health Potion on the ground!\n";
        hero.addItem(Item("Health Potion", "potion", 40));
        break;
    case 2:
        cout << "A traveling merchant offers you a rusty sword.\n";
        hero.addItem(Item("Rusty Sword", "weapon", 4));
        break;
    case 3: {
        int dmg = randInt(5, 15);
        cout << "You stumble into a trap! Take " << dmg << " damage.\n";
        hero.takeDamage(dmg);
        break;
    }
    case 4:
        cout << "A mystic healer fully restores 30 HP.\n";
        hero.gainXp(10);
        break;
    case 5:
        cout << "You discover an ancient tome (+15 XP).\n";
        hero.gainXp(15);
        break;
    case 6:
        cout << "Nothing happens. The wind howls...\n";
        break;
    }
    printLine('~');
    pause();
}
Character* createCharacter() {
    printLine('=');
    cout << "             MINI TEXT-BASED RPG   \n";
    printLine('=');
    cout << "Enter your hero's name: ";
    string name;
    getline(cin, name); 
    cout << "\n  Choose your class:\n"
         << "1. Warrior  (High HP & Defense, Power Strike)\n"
         << "2. Mage     (High Attack & Variance, Fireball)\n"
         << "> ";
    int cls; cin >> cls;
    cin.ignore();             
    if (cls == 2) return new Mage(name);
    return new Warrior(name);
}
int main() {
    srand(static_cast<unsigned>(time(nullptr)));
    Character* hero = createCharacter();
    hero->addItem(Item("Health Potion", "potion", 40));
    hero->addItem(Item("Health Potion", "potion", 40));
    hero->addItem(Item("Iron Sword",    "weapon", 5));
    int round = 0;
    const int maxRounds = 5;
    printLine('=');
    cout << "Your quest begins, " << hero->getName()
         << "the " << hero->getClass() << "!\n";
    printLine('=');
    pause();
    while (hero->isAlive() && round < maxRounds) {
        ++round;
        cout << "\n  === Round " << round << " / " << maxRounds << " ===\n";
        cout << "Choose:\n"
             << "1. Explore (fight enemy)\n"
             << "2. Random Event\n"
             << "3. View Status\n"
             << "4. Inventory\n"
             << "5. Quit\n"
             << "> ";
        int action; cin >> action;
        switch (action) {
        case 1: {
            Enemy* enemy = spawnEnemy(hero->getLevel());
            battle(*hero, *enemy);
            delete enemy;
            break;
        }
        case 2:
            randomEvent(*hero);
            break;
        case 3:
            hero->showStatus();
            pause();
            break;
        case 4:
            hero->showInventory();
            cout << "Use item? Enter number (0 to cancel): ";
            int idx; cin >> idx;
            if (idx > 0) hero->useItem(idx - 1);
            pause();
            break;
        case 5:
            cout << "Farewell, " << hero->getName() << "!\n";
            delete hero;
            return 0;
        default:
            cout << "Invalid choice.\n";
            --round;
        }
    }
    if (!hero->isAlive()) {
        printLine('=');
        cout << "GAME OVER  —  " << hero->getName() << " has fallen.\n";
    } else {
        printLine('=');
        cout << "QUEST COMPLETE!  " << hero->getName()
             << " survived " << maxRounds << " rounds!\n";
        cout << "Final Level: " << hero->getLevel() << "\n";
    }
    printLine('=');
    delete hero;
    return 0;
}
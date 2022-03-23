#include <iostream>
#include <vector>
#include <list>
#include <algorithm>

bool isTerminal(char c){
    return (!isupper(c) && c != '@' && c != '\'' && c != ' ');
}

struct ProductionRule{
    ProductionRule(std::string rule){
        auto rhsStartIndex = rule.find_first_of('>');

        if(rhsStartIndex != std::string::npos){
            this->lhs = rule[0];
            this->rhs = rule.substr(rhsStartIndex + 1, rule.size());
        }
        else{
            this->lhs = '`';
            this->rhs = rule;
        }
    }

    ProductionRule(const ProductionRule& p, char parserSymbol = '\0'){
        this->lhs = p.lhs;
        if(parserSymbol != '\0')
            this->rhs += parserSymbol;
        this->rhs += p.rhs;
    }

    ProductionRule(const ProductionRule& p, size_t parseSymbolIndex){
        this->lhs = p.lhs;
        this->rhs = p.rhs;
        this->rhs[parseSymbolIndex] = p.rhs[parseSymbolIndex + 1];
        this->rhs[parseSymbolIndex + 1] = p.rhs[parseSymbolIndex];
    }

    bool isLhs(char c){ 
        return lhs == c; 
    }

    friend bool operator==(const ProductionRule& l, const ProductionRule& r){
        if(l.lhs != r.lhs || l.rhs.size() != r.rhs.size())
            return false;

        for(auto i = 0; i < l.rhs.size(); i++){
            if(l.rhs[i] != r.rhs[i])
                return false;
        }

        return true;
    }

    std::size_t indexOfParseSymbol(char c){
        auto index = rhs.find(c);
        return index;
    }

    char symbolAtIndexAfterParse(char c){
        auto index = indexOfParseSymbol(c);
        return rhs[index + 1];
    }

    char valueAt(std::size_t i){ 
        return rhs[i]; 
    }

    std::string productionString(){
        std::string production = "";
        production += lhs;
        production += "->";
        production += rhs;
        return production;
    }
private:
    char lhs;
    std::string rhs;
};

struct Grammar{
    Grammar(std::vector<std::string> stringProductions){
        for(auto i : stringProductions){
            ProductionRule p = ProductionRule(i);
            rules.push_back(p);
        }
    }
    std::vector<ProductionRule> matchLhs(char c){
        std::vector<ProductionRule> p = std::vector<ProductionRule>();
        for(auto r : rules){
            if(r.isLhs(c))
                p.push_back(ProductionRule(r));
        }
        return p;
    }
    ProductionRule ruleAt(int i){
        ProductionRule p = ProductionRule(rules[i]);
        return p;
    }
    int ruleCount(){ return rules.size(); } 
private:
    std::vector<ProductionRule> rules;
};

struct Goto{
    Goto() : parentItemIndex(0), parentItemSymbol(' '), gotoItemIndex(0) {}

    Goto(size_t p, char c, size_t g = 0) : parentItemIndex(p), parentItemSymbol(c), gotoItemIndex(g) {}

    Goto(const Goto& g){
        this->parentItemIndex = g.parentItemIndex;
        this->parentItemSymbol = g.parentItemSymbol;
        this->gotoItemIndex = g.gotoItemIndex;
    }

    bool isSet(){ 
        return parentItemSymbol != ' '; 
    }

    bool isSymbol(char c){ 
        return parentItemSymbol == c; 
    }
    
    size_t gotoIndex(){ 
        return gotoItemIndex; 
    }

    char symbol(){
        return this->parentItemSymbol;
    }
private:
    size_t parentItemIndex;
    char parentItemSymbol;
    size_t gotoItemIndex;
};

struct Item{
    Item(Grammar g){
        addRule(g.ruleAt(0));
        closure(g);
        goTos = std::vector<Goto>(rules.size());
    }

    Item(std::vector<ProductionRule> rules){
        for(auto r : rules){
            auto parserSymbolIndex = r.indexOfParseSymbol('@');
            auto nextSymbol = r.valueAt(parserSymbolIndex + 1);

            ProductionRule newRule = ProductionRule(r, parserSymbolIndex);
            this->rules.push_back(newRule);
        }
        this->goTos = std::vector<Goto>(this->rules.size());
    }

    std::vector<ProductionRule> matchGoto(char c){
        std::vector<ProductionRule> p = std::vector<ProductionRule>();
        for(auto r : rules){
            auto parserSymbolIndex = r.indexOfParseSymbol('@');
            if(parserSymbolIndex != std::string::npos && r.valueAt(parserSymbolIndex + 1) == c)
                p.push_back(ProductionRule(r));
        }
        return p;
    }

    size_t ruleCount(){ 
        return rules.size(); 
    }

    friend bool operator==(const Item& l, const Item& r){
        if(l.rules.size() != r.rules.size())
            return false;
        
        for(auto i = 0; i < l.rules.size(); i++){
            if(l.rules[i] == r.rules[i])
                return true;
        }

        return false;
    }

    void setGoto(Goto g, int index){
        if(goTos[index].symbol() != g.symbol())
            goTos[index] = Goto(g);
    }

    bool hasGotoSymbol(char c){
        for(auto g : goTos){
            if(g.symbol() == c)
                return true;
        }
        return false;
    }

    ProductionRule ruleAt(int i){
        ProductionRule p = ProductionRule(rules[i]);
        return p;
    }

    Goto gotoAt(int i){
        Goto g = Goto(goTos[i]);
        return g;
    }

    void print(){
        for(auto i = 0; i < rules.size(); i++){
            if(goTos[i].isSet())
                std::printf("  %-20s goto(%c)=I%d\n", rules[i].productionString().c_str(), goTos[i].symbol(), goTos[i].gotoIndex());
            
            else
                std::printf("  %-20s\n", rules[i].productionString().c_str());
            
        }
    }
private:
    void addRule(ProductionRule p){
        if(p.indexOfParseSymbol('@') == std::string::npos)
            p = ProductionRule(p, '@');

        for(auto r : rules){
            if(r == p){
                return;
            }
        }
        rules.push_back(p);
    }

    void closure(Grammar g){
        auto index = 0;

        while(index < rules.size()){
            auto currentRule = rules[index];
            auto parseSymbolIndex = currentRule.indexOfParseSymbol('@');

            if(parseSymbolIndex != std::string::npos){
                char nextIndexChar = currentRule.valueAt(parseSymbolIndex + 1);
                if(!isTerminal(nextIndexChar)){
                    std::vector<ProductionRule> matchedRules = g.matchLhs(nextIndexChar);
                    for(auto i : matchedRules){
                        ProductionRule p = ProductionRule(i, '@');
                        addRule(p);
                    }
                }
            }

            index++;
        }
    }
    std::vector<ProductionRule> rules;
    std::vector<Goto> goTos;
};

struct Set{
    Set(Item i){
        items.push_back(i);
        goTos();
    }
    void print(){
        for(auto i = 0; i < items.size(); i++){
            std::printf("I%d:\n", i);
            items[i].print();
        }
    }
private:
    void goTos(){
        auto itemIndex = 0;

        while(itemIndex < items.size()){
            Item currentItem = items[itemIndex];
            for(auto ruleIndex = 0; ruleIndex < currentItem.ruleCount(); ruleIndex++){
                ProductionRule currentRule = currentItem.ruleAt(ruleIndex);
                Goto currentGoto = currentItem.gotoAt(ruleIndex);
                size_t parseSymbolIndex = currentRule.indexOfParseSymbol('@');
                char nextIndexSymbol = currentRule.valueAt(parseSymbolIndex + 1);

                if(!isTerminal(nextIndexSymbol)){                 
                    std::vector<ProductionRule> matchedGotoRules = items[itemIndex].matchGoto(nextIndexSymbol);
                    items.push_back(Item(matchedGotoRules));
                    if(!currentItem.hasGotoSymbol(nextIndexSymbol))
                        items[itemIndex].setGoto(Goto(ruleIndex, nextIndexSymbol, items.size()), ruleIndex);
                }
            }
            itemIndex++;
        }
    }
    std::vector<Item> items;
};


std::vector<std::string> readGrammar(){
    std::vector<std::string> lines;
    std::string readLine;

    /** Read grammar production rules for stdin */
    while(std::getline(std::cin, readLine))
        lines.push_back(readLine);
    
    return lines;
}


int main(int argc, char *argv[]){
    std::vector<std::string> lines = readGrammar();
    Grammar grammar = Grammar(lines);
    Item item = Item(grammar);
    Set set = Set(item);
    set.print();
    return 0;
}
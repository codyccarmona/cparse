/**
 *  Assignment: MP3 - C parse
 *  Subject: read in a start symbol of a set of productions followed by the set of productions
 *       from  stdin  and  produce  the  collection  of  sets  of  LR(0)  items  to  stdout
 *  Author: Cody Carmona
 *  FSUid: ccc13d
 *  Compile command: g++ -std=c++11 -Wall -Wextra -o carmona.exe carmona.cpp
 */

#include <iostream>
#include <vector>
#include <list>
#include <algorithm>


struct ProductionRule{
    ProductionRule(){ }

    ProductionRule(std::string rule, char p){
        int productionStartIndex = rule.find_first_of('>');
        std::string formattedRule = formatRule(rule);
        if(productionStartIndex != std::string::npos){
            lhs = rule[0];
            productionStartIndex++;
        }
        else{
            lhs = '`';
            productionStartIndex = 0;
        }

        rhs = p;
        rhs += rule.substr(productionStartIndex, rule.size());
        parserSymbol = p;
    }

    ProductionRule(char l, std::string r, char p){
        lhs = l;
        rhs = r;
        parserSymbol = p;
    }

    ProductionRule(const ProductionRule& p){
        this->lhs = p.lhs;
        this->rhs = p.rhs;
        this->parserSymbol = p.parserSymbol;
    }

    ProductionRule advanceParserSymbol(){
        ProductionRule p = ProductionRule(lhs, rhs, parserSymbol);
        auto parserSymbolIndex = p.indexOfParseSymbol();

        p.rhs[parserSymbolIndex] = p.charAfterParseSymbol();;
        p.rhs[parserSymbolIndex + 1] = p.parserSymbol;  

        return p;
    }

    bool matchesLhs(char c){ 
        return lhs == c; 
    }

    friend bool operator==(const ProductionRule& l, const ProductionRule& r){
        if(l.lhs != r.lhs || l.rhs.size() != r.rhs.size())
            return false;

        return l.rhs == r.rhs;
    }

    friend bool operator!=(const ProductionRule& l, const ProductionRule& r){
        return !(l == r);
    }

    char charAfterParseSymbol(){
        auto index = indexOfParseSymbol();
        return rhs[index + 1];
    }

    std::string productionString(){
        std::string production = "";
        production += lhs;
        production += + "->";
        production += rhs;

        return production;
    }

    bool isNonterminal(char c){
        return (std::isalpha(c) && std::isupper(c));
    }

    bool isTerminal(char c){
        return (!isNonterminal(c) && c != '@' && c != '\'' && c != '\0');
    }

private:
    std::string formatRule(std::string rule){
        if(rule.find(' ') == std::string::npos)
            return rule;
        
        std::string formattedRule = "";
        for(auto ruleItr = rule.begin(); ruleItr != rule.end(); ruleItr++){
            if(std::isspace(*ruleItr))
                continue;
            formattedRule += *ruleItr;
        }
        return formattedRule;
    }

    int indexOfParseSymbol(){
        auto index = rhs.find(parserSymbol);
        return index;
    }
    char lhs;
    std::string rhs;
    char parserSymbol;
};

struct AugmentedGrammar{
    AugmentedGrammar(std::vector<std::string> stringProductions){
        for(auto i : stringProductions){
            ProductionRule rule = ProductionRule(i, '@');
            rules.push_back(rule);
        }
    }

    std::vector<ProductionRule> getRulesFor(char c){
        std::vector<ProductionRule> p = std::vector<ProductionRule>();
        for(auto r : rules){
            if(r.matchesLhs(c))
                p.push_back(ProductionRule(r));
        }
        return p;
    }

    ProductionRule ruleAtIndex(int i){
        ProductionRule p = ProductionRule(rules[i]);
        return p;
    }

    int ruleCount(){ 
        return rules.size(); 
    } 


    void print(){
        std::printf("Augmented Grammar\n");
        std::printf("-----------------\n");
        for(auto r : rules){
            std::printf("%s\n", r.productionString().c_str());
        }
        std::printf("\n");
    }
private:
    std::vector<ProductionRule> rules;
};

struct Goto{
    Goto(char s, int i){
        symbol = s;
        sourceRuleIndex = i;
    }

    void setdestinationRuleIndex(int i){
        destinationRuleIndex = i;
    }

    int getSourceRuleIndex(){
        return sourceRuleIndex;
    }

    int getDestinationRuleIndex(){
        return destinationRuleIndex;
    }

    friend bool operator==(const Goto& l, const Goto& r){
        return l.symbol == r.symbol;
    }

    char getSymbol(){
        return symbol;
    }
private:
    char symbol;
    int sourceRuleIndex;
    int destinationRuleIndex;
};

struct Item{
    Item(AugmentedGrammar grammar, ProductionRule initialRule){
        addRule(initialRule);
        closure(grammar, 0);
    }

    Item(Item item, int gotoIndex, AugmentedGrammar grammar){
        std::vector<ProductionRule> matchedRules;

        for(auto r : item.rules){
            if(r.charAfterParseSymbol() == item.gotos[gotoIndex].getSymbol())
                addRule(r.advanceParserSymbol());
        }
        closure(grammar, 0);
    }

    int ruleCount(){ 
        return rules.size(); 
    }

    int gotoCount(){
        return gotos.size();
    }

    friend bool operator==(const Item& l, const Item& r){
        if(l.rules.size() != r.rules.size())
            return false;
        
        for(auto i = 0; i < l.rules.size(); i++){
            if(l.rules[i] != r.rules[i])
                return false;
        }

        return true;
    }

    void setDestinationForGoto(int gotoIndex, int destinationRuleIndex){
        gotos[gotoIndex].setdestinationRuleIndex(destinationRuleIndex);
    }

    friend bool operator!=(const Item& l, const Item& r){
        return !(l == r);
    }

    void print(){
        for(auto i = 0; i < rules.size(); i++){
            int matchedGotoIndex = findGotoWithSourceRuleIndex(i);

            if(matchedGotoIndex >= 0)
                std::printf("  %-20s goto(%c)=I%d\n", rules[i].productionString().c_str(), gotos[matchedGotoIndex].getSymbol(), gotos[matchedGotoIndex].getDestinationRuleIndex());
            
            else
                std::printf("  %-20s\n", rules[i].productionString().c_str());
        }
    }
private:
    int findGotoWithSourceRuleIndex(int i){
        for(auto j = 0; j < gotos.size(); j++){
            if(gotos[j].getSourceRuleIndex() == i)
                return j;
        }

        return -1;
    }

    void addRule(ProductionRule p){
        for(auto r : rules){
            if(r == p) 
                return;
        }
        rules.push_back(p);
    }

    void closure(AugmentedGrammar grammar, int currentRuleIndex){
        if(currentRuleIndex >= rules.size())
            return;

        char charAfterParserSymbol = rules[currentRuleIndex].charAfterParseSymbol();
        bool isNonTerminal = rules[currentRuleIndex].isNonterminal(charAfterParserSymbol);

        if(charAfterParserSymbol != '\0')
            addGoto(Goto(charAfterParserSymbol, currentRuleIndex));
            
        if(isNonTerminal){
            std::vector<ProductionRule> matchedRules = grammar.getRulesFor(charAfterParserSymbol);
            for(auto mr : matchedRules)
                addRule(mr);
        }
        closure(grammar, currentRuleIndex + 1);
    }

    void addGoto(Goto goTo){
        for(auto g : gotos){
            if(g == goTo)
                return;
        }
        gotos.push_back(goTo);
    }

    std::vector<Goto> gotos;
    std::vector<ProductionRule> rules;
};

struct Set{
    Set(AugmentedGrammar grammar){
        items.push_back(Item(grammar, grammar.ruleAtIndex(0)));
        go_to(grammar, 0);
    }

    void print(){
        std::printf("Sets of LR(0) Items\n");
        std::printf("-------------------\n");
        for(int i = 0; i < items.size(); i++){
            std::printf("I%d:\n", i);
            items[i].print();
            std::printf("\n");
        }
    }
private:
    void go_to(AugmentedGrammar grammar, int currentItemIndex){
        if(currentItemIndex >= items.size())
            return;

        for(int gotoIndex = 0; gotoIndex < items[currentItemIndex].gotoCount(); gotoIndex++){      
            Item item = Item(items[currentItemIndex], gotoIndex, grammar);
            int foundItemIndex = findItem(item);

            if(foundItemIndex < 0){
                items.push_back(item);
                items[currentItemIndex].setDestinationForGoto(gotoIndex ,items.size() - 1);
            }
            else
                items[currentItemIndex].setDestinationForGoto(gotoIndex, foundItemIndex);              
        }      

        go_to(grammar, currentItemIndex + 1);  
    }
    
    int findItem(Item item){
        for(int i = 0; i < items.size(); i++){
            if(items[i] == item)
                return i;
        }
        return -1;
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
    std::vector<std::string> grammarLines = readGrammar();
    AugmentedGrammar augmentedGrammar = AugmentedGrammar(grammarLines);
    Set set = Set(augmentedGrammar);
    augmentedGrammar.print();
    set.print();
    return 0;
}
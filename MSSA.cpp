#include "source/MSSA.h"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <math.h>

using namespace std::placeholders;

bool AStar::coords::operator == (const coords& coordinates_)
{
    /// dodany 3 wymiar
    return (x == coordinates_.x && y == coordinates_.y && z == coordinates_.z);
}

AStar::Node::Node(coords coordinates_, Node* parent_)
{
    parent = parent_;
    coordinates = coordinates_;
    G = H = 0;
}

/*AStar::uint*/ float AStar::Node::getScore()
{
    return G + H;
    return total;
}
/// MSSA obliczanie wag
void AStar::Node::calcTotal(const float &dst) {
    float whmax=0.8, whmin=0.2, lambda=1;
    float wh = whmin - pow((whmax - whmin), -(lambda*(H/dst)));
    float wg = 1 - wh;
    total = wh*G+wg*H;
}

AStar::Generator::Generator()
{
    //setDiagonalMovement(false);
    setHeuristic(&Heuristic::manhattan);

    /// kierunki dla przestrzeni 3 wymiarowej
    direction = {
       { 1 ,0 ,0 }, { 2, 0, 0 }, {-1 ,0 , 0 }, {-2 ,0 , 0 },
       { 0, 1, 0 }, { 0, 2, 0 }, { 0, -1, 0 }, { 0, -2, 0 },
       { 0 ,0 ,1 }, { 0 ,0 ,2 }, { 0 ,0 ,-1 }, { 0 ,0 ,-2 }      
    };
}

void AStar::Generator::setWorldSize(coords worldSize_)
{
    wallsGrid.resize(worldSize_.x, std::vector<std::vector<bool>>(worldSize_.y, std::vector<bool>(worldSize_.z, false)));
    worldSize = worldSize_;
}

//void AStar::Generator::setDiagonalMovement (bool enable_)
//{
//    directions = (enable_ ? 32 : 12);
//}

void AStar::Generator::setHeuristic(HeuristicFunction heuristic_)
{
    heuristic = std::bind(heuristic_, _1, _2);
}

void AStar::Generator::addCollision(coords coordinates_)
{
    wallsGrid[coordinates_.x][coordinates_.y][coordinates_.z] = true;
}

void AStar::Generator::removeCollision(coords coordinates_)
{
    auto it = std::find(walls.begin(), walls.end(), coordinates_);
    if (it != walls.end()) {
        walls.erase(it);
    }
}

void AStar::Generator::clearCollisions()
{
    /*std::transform(wallsGrid.begin(), wallsGrid.end(), wallsGrid.begin(),
        [](bool& p) {
            p = false;
            return p;
        });*/
}

void AStar::Generator::load(std::string fname) {
    std::ifstream file(fname);
    if (!file.is_open()) {
        std::cout << "failed to open file" << fname << '\n';
        return;
    }
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::vector<int> lineValues;
        std::string value;
        int counter = 1;
        while (std::getline(ss, value, ',')) {
            int val = std::stoi(value);
            if (val < 0 || val>19) {
                std::cout << "incorrect coordinates at line " << counter << '\n';
                ++counter;
                continue;
            }
            lineValues.push_back(val);
            ++counter;
        }
        addCollision({ lineValues[0],lineValues[1],lineValues[2] });
    }
    file.close();
}

AStar::CoordinateList AStar::Generator::findPath(coords source_, coords target_)
{
    ///wyliczanie dst dla wagowania
    dst = AStar::Heuristic::manhattan(source_,target_);


    Node* current = nullptr;
    NodeSet openSet, closedSet;
    openSet.push_back(new Node(source_));
    
    while (!openSet.empty()) {
        auto current_it = openSet.begin();
        current = *current_it;
        int counter = 0;
        for (auto it = openSet.begin(); it != openSet.end()/*&&counter<openSize*/; it++,counter++) {
            
            auto node = *it;
            if (node->getScore() <= current->getScore()) {
                current = node;
                current_it = it;
            }
        }

        if (current->coordinates == target_) {
            std::cout << "path found";
            break;
        }

        closedSet.push_back(current);

        //optymalizacja algorytmiczna
        //openSet.erase(current_it);
        std::swap(*current_it, openSet.back());
        openSet.pop_back();
        
        for (uint i = 0; i < directions; i += 2) {
            
            coords newCoordinates(current->coordinates + direction[i]);
          
            if (detectCollision(newCoordinates) ||
                findNodeOnList(closedSet, newCoordinates))
            {
                continue;
            }
            
            //double layer etension
            coords DoubleLayer(current->coordinates + direction[i + 1]);
            bool DLJump = false;
            if (!(detectCollision(DoubleLayer) ||
                findNodeOnList(closedSet, DoubleLayer))) {
                if (heuristic(DoubleLayer, target_) < heuristic(newCoordinates, target_)) {
                    newCoordinates = DoubleLayer;
                    DLJump = true;
                }
            }
            
            uint totalCost = current->G;
            Node* successor = findNodeOnList(openSet, newCoordinates);

            if (successor == nullptr) {
                successor = new Node(newCoordinates, current);
                successor->G = totalCost+10+ DLJump;
                successor->H = heuristic(successor->coordinates, target_); 
                
                //moze nie byc tutaj
                successor->calcTotal(dst);

                openSet.push_back(successor);
                
                
               
            }
            else if (totalCost < successor->G) {
                successor->parent = current;
                successor->G = totalCost+10+ DLJump;
            }
        }
        
    }

    CoordinateList path;
    while (current != nullptr) {
        path.push_back(current->coordinates);
        current = current->parent;
    }

    releaseNodes(openSet);
    releaseNodes(closedSet);

    return path;
}

AStar::Node* AStar::Generator::findNodeOnList(NodeSet& nodes_, coords coordinates_)
{
    for (auto node : nodes_) {
        if (node->coordinates == coordinates_) {
            return node;
        }
    }
    return nullptr;
}

void AStar::Generator::releaseNodes(NodeSet& nodes_)
{
    for (auto it = nodes_.begin(); it != nodes_.end();) {
        delete* it;
        it = nodes_.erase(it);
    }
}

bool AStar::Generator::detectCollision(coords coordinates_)
{
    if (coordinates_.x < 0 || coordinates_.x >= worldSize.x ||
        coordinates_.y < 0 || coordinates_.y >= worldSize.y ||
        coordinates_.z < 0 || coordinates_.z >= worldSize.z ||
        wallsGrid[coordinates_.x][coordinates_.y][coordinates_.z]) {
        return true;
    }
    return false;
}

AStar::coords AStar::Heuristic::getDelta(coords source_, coords target_)
{
    return{ abs(source_.x - target_.x),  abs(source_.y - target_.y), abs(source_.z - target_.z) };
}

AStar::uint AStar::Heuristic::manhattan(coords source_, coords target_)
{
    auto delta = std::move(getDelta(source_, target_));
    return static_cast<uint>(10*(delta.x + delta.y+ delta.z));
}

AStar::uint AStar::Heuristic::euclidean(coords source_, coords target_)
{
    auto delta = std::move(getDelta(source_, target_));
    return static_cast<uint>(10*sqrt(pow(delta.x, 2) + pow(delta.y, 2) + pow(delta.z, 2)));
}

AStar::uint AStar::Heuristic::octagonal(coords source_, coords target_)
{
    auto delta = std::move(getDelta(source_, target_));
    return 10 * (delta.x + delta.y) + (-6) * std::min(delta.x, delta.y);
}
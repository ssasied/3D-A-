/*
    Copyright (c) 2015, Damian Barczynski <daan.net@wp.eu>
    Following tool is licensed under the terms and conditions of the ISC license.
    For more information visit https://opensource.org/licenses/ISC.
*/

#ifndef __ASTAR_HPP_8F637DB91972F6C878D41D63F7E7214F__
#define __ASTAR_HPP_8F637DB91972F6C878D41D63F7E7214F__

#include <vector>
#include <functional>
#include <set>
#include <string>

namespace AStar
{
    struct coords
    {
        /// dodany 3 wymiar
        int x, y,z;

        bool operator == (const coords& coordinates_);
        friend coords operator + (const AStar::coords& left_, const AStar::coords& right_) {
            ///dodany 3 wymiar
            return{ left_.x + right_.x, left_.y + right_.y ,left_.z+right_.z};
        }
    };

    using uint = unsigned int;
    using HeuristicFunction = std::function<uint(coords, coords)>;
    using CoordinateList = std::vector<coords>;

    struct Node
    {
        uint G, H;

        /// zmiana dla MSSA
        float total;

        coords coordinates;
        Node* parent;

        Node(coords coord_, Node* parent_ = nullptr);
        
        /// zmieniona dla MSSA
        float /*uint*/ getScore();

        void calcTotal(const float& dst);
    };

    using NodeSet = std::vector<Node*>;

    class Generator
    {
        bool detectCollision(coords coordinates_);
        Node* findNodeOnList(NodeSet& nodes_, coords coordinates_);
        void releaseNodes(NodeSet& nodes_);
    public:
        float dst;
        std::vector<std::vector<std::vector<bool>>> wallsGrid;
        Generator();
        
        ///zmieniona dla mojego podjescia z wektorem 3 wymiarowym jako przestrzen
        void setWorldSize(coords worldSize_);

        /// moja funkcja do ³adowania przeszkód z pliku
        void load(std::string fname);
        
        ///wyrzucowa funkcja bo sprawienie aby ona dzia³a³a w 3 wymiarach jest zbyt pracoch³onne
        //void setDiagonalMovement(bool enable_);

        void setHeuristic(HeuristicFunction heuristic_);

        CoordinateList findPath(coords source_, coords target_);

        /// zmienione dla mojego podejscia
        void addCollision(coords coordinates_);
        void removeCollision(coords coordinates_);
        void clearCollisions();

    private:
        HeuristicFunction heuristic;
        CoordinateList direction, walls;
        coords worldSize;

        /// ilosc zmieniona dla 3 wymiarow
        uint directions=12;
       
    };

    class Heuristic
    {
        static coords getDelta(coords source_, coords target_);

    public:
        static uint manhattan(coords source_, coords target_);
        static uint euclidean(coords source_, coords target_);
        static uint octagonal(coords source_, coords target_);
    };
}

#endif // __ASTAR_HPP_8F637DB91972F6C878D41D63F7E7214F__
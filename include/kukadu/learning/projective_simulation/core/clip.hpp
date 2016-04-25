#ifndef KUKADU_CLIP_H
#define KUKADU_CLIP_H

#include <set>
#include <string>
#include <vector>
#include <limits>
#include <utility>
#include <sstream>
#include <limits.h>
#include <iostream>
#include <algorithm>

#include <kukadu/types/kukadutypes.hpp>

namespace kukadu {

    class Clip : public KUKADU_ENABLE_SHARED_FROM_THIS<Clip> {

    private:

        static constexpr int NEXT_HOP_NOT_PREDEF = -1;

        int level;
        int immunity;
        int gotDeleted;
        int previousRank;
        int initialImmunity;

        int nextHop;

        KUKADU_SHARED_PTR<kukadu_mersenne_twister> generator;

        std::vector<double> subH;

        KUKADU_DISCRETE_DISTRIBUTION<int> discDist;

        KUKADU_SHARED_PTR<std::set<KUKADU_SHARED_PTR<Clip> > > parents;
        KUKADU_SHARED_PTR<std::set<KUKADU_SHARED_PTR<Clip> > > subClipsSet;

        KUKADU_SHARED_PTR<std::vector<int> > clipDimensionValues;
        KUKADU_SHARED_PTR<std::vector<KUKADU_SHARED_PTR<Clip> > > subClips;

        void construct(int level, KUKADU_SHARED_PTR<kukadu_mersenne_twister> generator, KUKADU_SHARED_PTR<std::vector<int> > clipValues, int immunity);

    protected:

        int visitedSubNode;

    public:

        Clip(int level, KUKADU_SHARED_PTR<kukadu_mersenne_twister> generator, std::string clipValues, int immunity);
        Clip(int level, KUKADU_SHARED_PTR<kukadu_mersenne_twister> generator, KUKADU_SHARED_PTR<std::vector<int> > clipValues, int immunity);
        ~Clip();

        // must set the visitedSubnode member if overwritten --> otherwise it will not update its weights
        // todo: remove that requirement that subclasses have to set that by themselves
        virtual std::pair<int, KUKADU_SHARED_PTR<Clip> > jumpNextRandom();

        static KUKADU_SHARED_PTR<std::vector<int> > getIdVectorFromString(std::string str);
        static bool compareIdVecs(const KUKADU_SHARED_PTR<std::vector<int> > vec1, const KUKADU_SHARED_PTR<std::vector<int> > vec2);

        void printSubWeights();
        void setPreviousRank();
        void decreaseImmunity();
        void removeAllSubClips();
        void initRandomGenerator();
        void setImmunity(int immunity);
        void addParent(KUKADU_SHARED_PTR<Clip> par);
        void addHAndInitialize(int idx, int addWeight);
        void updateWeights(double reward, double gamma);
        void removeSubClip(KUKADU_SHARED_PTR<Clip> clip);
        void removeParentClip(KUKADU_SHARED_PTR<Clip> c);
        void addChildUpwards(KUKADU_SHARED_PTR<Clip> sub);
        void addParentDownwards(KUKADU_SHARED_PTR<Clip> par);
        void removeSubClipWoRand(KUKADU_SHARED_PTR<Clip> clip);
        void addSubClip(KUKADU_SHARED_PTR<Clip> sub, int weight);
        void setClipDimensionValues(KUKADU_SHARED_PTR<std::vector<int> > vals);
        void setChildren(KUKADU_SHARED_PTR<std::vector<KUKADU_SHARED_PTR<Clip> > > children);
        void setChildren(KUKADU_SHARED_PTR<std::vector<KUKADU_SHARED_PTR<Clip> > > children, std::vector<double> weights);

        bool isImmune();

        virtual bool isCompatibleSubclip(KUKADU_SHARED_PTR<Clip> c);

        int getLevel();
        int getPreviousRank();
        int getSubClipCount();
        int getDimensionality();
        int getInitialImmunity();
        int getCurrentImmunity();

        int getSubClipIdx(KUKADU_SHARED_PTR<Clip> subClip);

        double getWeightByIdx(int idx);
        double computeSubEntropy() const;

        virtual double computeRank() const;

        std::string getIdVecString() const;

        virtual std::string toString() const;

        void setNextHop(int hopIdx);
        void setNextHop(KUKADU_SHARED_PTR<Clip> hopClip);

        KUKADU_SHARED_PTR<std::vector<KUKADU_SHARED_PTR<Clip> > > getSubClips();

        KUKADU_SHARED_PTR<Clip> getLikeliestChild();
        KUKADU_SHARED_PTR<Clip> getSubClipByIdx(int idx);
        KUKADU_SHARED_PTR<Clip> getSubClipByLabel(std::string idx);
        KUKADU_SHARED_PTR<std::vector<int> > getClipDimensions() const;
        KUKADU_SHARED_PTR<Clip> compareClip(KUKADU_SHARED_PTR<Clip> c);
        KUKADU_SHARED_PTR<std::set<KUKADU_SHARED_PTR<Clip> > > getParents();

        friend bool operator< (const Clip &o1, const Clip &o2);
        friend bool operator== (const Clip &o1, const Clip &o2);
        friend bool operator!= (const Clip &o1, const Clip &o2);

        friend std::ostream& operator<<(std::ostream &strm, const Clip &c);

        // start weight according to the paper
        static const int CLIP_H_STD_WEIGHT;
        static const int CLIP_H_HASH_VAL;
        static const int CLIP_H_LEVEL_FINAL;
        static const int CLIP_H_NOT_WALKED_YET;

        static const int PS_DEFAULT_IMMUNITY;

    };

}

#endif

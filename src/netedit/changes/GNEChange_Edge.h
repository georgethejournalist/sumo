/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEChange_Edge.h
/// @author  Jakob Erdmann
/// @date    Mar 2011
///
// A network change in which a single edge is created or deleted
/****************************************************************************/
#ifndef GNEChange_Edge_h
#define GNEChange_Edge_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <fx.h>
#include <utils/foxtools/fxexdefs.h>
#include <vector>
#include <map>
#include "GNEChange.h"

// ===========================================================================
// class declarations
// ===========================================================================
class GNENet;
class GNEEdge;
class GNELane;
class GNEAdditional;
class GNERerouter;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEChange_Edge
 * A network change in which a single edge is created or deleted
 */
class GNEChange_Edge : public GNEChange {
    // @brief FOX Declaration
    FXDECLARE_ABSTRACT(GNEChange_Edge)

public:
    /**@brief Constructor for creating/deleting an edge
     * @param[in] edge The edge to be created/deleted
     * @param[in] forward Whether to create/delete (true/false)
     */
    GNEChange_Edge(GNEEdge* edge, bool forward);

    /// @brief Destructor
    ~GNEChange_Edge();

    /// @name inherited from GNEChange
    /// @{
    /// @brief get undo Name
    FXString undoName() const;

    /// @brief get Redo name
    FXString redoName() const;

    /// @brief undo action
    void undo();

    /// @brief redo action
    void redo();
    /// @}


private:
    /**@brief full information regarding the edge that is to be created/deleted
     * @note we assume shared responsibility for the pointer (via reference counting)
     */
    GNEEdge* myEdge;

    /// @brief reference to vector of parent shapes
    const std::vector<GNEShape*>& myEdgeParentShapes;

    /// @brief reference to vector of parent additionals
    const std::vector<GNEAdditional*>& myEdgeParentAdditionals;

    /// @brief reference to vector of parent demand elements
    const std::vector<GNEDemandElement*>& myEdgeParentDemandElements;

    /// @brief reference to vector of child shapes
    const std::vector<GNEShape*>& myChildEdgeShapes;

    /// @brief reference to vector of child additional
    const std::vector<GNEAdditional*>& myChildEdgeAdditionals;

    /// @brief vector of references to vector of child demand elements (used by lanes)
    const std::vector<GNEDemandElement*>& myChildEdgeDemandElements;

    /// @brief vector of references to vector of parent shapes (used by lanes)
    std::vector<std::vector<GNEShape*> > myLaneParentShapes;

    /// @brief vector of references to vector of parent additionals (used by lanes)
    std::vector<std::vector<GNEAdditional*> > myLaneParentAdditionals;

    /// @brief vector of references to vector of parent demand elements (used by lanes)
    std::vector<std::vector<GNEDemandElement*> > myLaneParentDemandElements;

    /// @brief vector of references to vector of child shapes (used by lanes)
    std::vector<std::vector<GNEShape*> > myChildLaneShapes;

    /// @brief vector of references to vector of child additional (used by lanes)
    std::vector<std::vector<GNEAdditional*> > myChildLaneAdditionals;

    /// @brief vector of references to vector of child demand elements (used by lanes)
    std::vector<std::vector<GNEDemandElement*> > myChildLaneDemandElements;
};

#endif
/****************************************************************************/

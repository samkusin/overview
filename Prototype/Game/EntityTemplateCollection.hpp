//
//  EntityTemplateCollection.h
//  Overview
//
//  Created by Samir Sinha on 10/12/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef Overview_Game_Model_EntityTemplateCollection_Hpp
#define Overview_Game_Model_EntityTemplateCollection_Hpp

#include "Game/EntityTemplate.hpp"
#include "Engine/Model/ModelCollection.hpp"
#include "cinek/string.hpp"

namespace cinekine { namespace ovengine {
    
    class EntityTemplateCollection : public ModelCollection<EntityTemplate>
    {
    public:
        /// Default constructor
        ///
        EntityTemplateCollection() = default;
        /// Constructs a Block collection
        ///
        /// @param name               The collection name
        /// @param blocks             The collection's blocks
        EntityTemplateCollection(const char* name,
                                 vector<EntityTemplate>&& templates);
        /// Returns the tile at the requested index.
        /// @param  name The name of the Block
        /// @return The mapped Block object
        ///
        const Type& operator[](const std::string& name) const;
            
            
    private:
        EntityTemplate _nullEntity;
    };
    
} /* namespace ovengine */ } /* namespace cinekine */


#endif /* Overview_Game_Model_EntityTemplateCollection_Hpp */

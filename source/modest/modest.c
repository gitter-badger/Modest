/*
 Copyright (C) 2016 Alexander Borisov
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 
 Author: lex.borisov@gmail.com (Alexander Borisov)
*/

#include "modest/modest.h"
#include "modest/style/sheet.h"
#include "modest/style/raw.h"
#include "modest/node/node.h"

modest_t * modest_create(void)
{
    return (modest_t*)myhtml_calloc(1, sizeof(modest_t));
}

modest_status_t modest_init(modest_t* modest)
{
    /* Modest nodes */
    modest->mnode_obj = mcobject_async_create();
    if(modest->mnode_obj == NULL)
        return MODEST_STATUS_OK;
    
    mcobject_async_status_t mcstatus = mcobject_async_init(modest->mnode_obj, 128, 1024, sizeof(modest_node_t));
    if(mcstatus)
        return MODEST_STATUS_OK;
    
    /* base object node for all modest node objects */
    modest->mnode_node_id = mcobject_async_node_add(modest->mnode_obj, &mcstatus);
    
    if(mcstatus)
        return MODEST_STATUS_OK;
    
    
    /* Modest stylesheet */
    modest->mstylesheet_obj = mcobject_async_create();
    if(modest->mstylesheet_obj == NULL)
        return MODEST_STATUS_OK;
    
    mcstatus = mcobject_async_init(modest->mstylesheet_obj, 128, 1024, sizeof(modest_style_sheet_t));
    if(mcstatus)
        return MODEST_STATUS_OK;
    
    /* base object node for all modest stylesheet objects */
    modest->mstylesheet_node_id = mcobject_async_node_add(modest->mstylesheet_obj, &mcstatus);
    
    if(mcstatus)
        return MODEST_STATUS_OK;
    
    
    /* Modest style type */
    modest->mstyle_type_obj = mchar_async_create(12, (4096 * 5));
    if(modest->mstyle_type_obj == NULL)
        return MODEST_STATUS_OK;
    
    modest->mstyle_type_node_id = mchar_async_node_add(modest->mstyle_type_obj);
    
    
    /* for raw declaration style */
    modest->mraw_style_declaration_obj = mcobject_create();
    if(modest->mraw_style_declaration_obj == NULL)
        return MODEST_STATUS_OK;
    
    myhtml_status_t myhtml_status = mcobject_init(modest->mraw_style_declaration_obj, 256, sizeof(modest_style_raw_declaration_t));
    if(myhtml_status)
        return MODEST_STATUS_OK;
    
    /* layers */
    modest->layout = modest_layers_create();
    if(modest->layout == NULL)
        return MODEST_STATUS_ERROR_MEMORY_ALLOCATION;
    
    modest_status_t modest_status = modest_layers_init(modest->layout);
    if(modest_status)
        return MODEST_STATUS_ERROR;
    
    modest->style_avl_tree = myhtml_utils_avl_tree_create();
    if(modest->style_avl_tree == NULL)
        return MODEST_STATUS_ERROR_MEMORY_ALLOCATION;
    
    myhtml_status = myhtml_utils_avl_tree_init(modest->style_avl_tree);
    if(myhtml_status)
        return MODEST_STATUS_ERROR;
    
    return MODEST_STATUS_OK;
}

void modest_clean(modest_t* modest)
{
    mcobject_async_clean(modest->mnode_obj);
    mcobject_async_clean(modest->mstylesheet_obj);
    modest_layers_clean_all(modest->layout);
    myhtml_utils_avl_tree_clean(modest->style_avl_tree);
}

modest_t * modest_destroy(modest_t* modest, bool self_destroy)
{
    if(modest == NULL)
        return NULL;
    
    modest->mnode_obj = mcobject_async_destroy(modest->mnode_obj, true);
    modest->mstylesheet_obj = mcobject_async_destroy(modest->mstylesheet_obj, true);
    modest->layout = modest_layers_destroy(modest->layout, true);
    modest->style_avl_tree = myhtml_utils_avl_tree_destroy(modest->style_avl_tree, true);
    
    if(self_destroy) {
        myhtml_free(modest);
        return NULL;
    }
    
    return modest;
}



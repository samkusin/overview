bl_info = {
    "name" : "Export OVEngine Objects (.json)",
    "author" : "Samir Sinha",
    "version" : (0,0,3),
    "blender" : (2,7,5),
    "category" : "Import-Export"
}

import bpy, mathutils, math
import json, numpy, os
from collections import namedtuple, OrderedDict

def is_face_degenerate(face):
    if len(face.vertices) < 3:
        return True
    elif face.vertices[0] == face.vertices[1]:
        return True
    elif face.vertices[1] == face.vertices[2]:
        return True
    elif face.vertices[2] == face.vertices[0]:
        return True
    return False


def matrix_to_list(matrix):
    l = []
    # Let's hope Blender doesn't change the Matrix spec yet again
    # At least its easier to select whether we want to return column or row major
    # cell lists
    for col in matrix.col:
        for rowcell in col:
            l.append(rowcell)
    return l


def make_texture_path(root, filepath):
    path = os.path.join(root, filepath.lstrip('/'))
    return os.path.normpath(path)


def generate_mesh_elements(obj, mesh, resources):
    mesh_name = obj.name
    mesh_vertices = mesh.vertices
    mesh_faces = list(mesh.polygons)
    mesh_uv_textures = mesh.tessface_uv_textures

    face_has_uvs = len(mesh_uv_textures) > 0    # UV map
    has_weights = '_armature' in resources and obj.vertex_groups

    Vertex = namedtuple('Vertex', 'id pos normal weights uv subvertices')

    our_materials = resources['materials']
    our_mesh_elements = []
    has_ovobject_props = hasattr(obj, 'ovobject_type_property')
    is_viewable = not has_ovobject_props or obj.ovobject_type_property == 'View'
    face_has_uvs = face_has_uvs and is_viewable

    while mesh_faces:
        # Subdivide the Mesh into MeshElements (An element being a mesh
        # with a single common material)
        #
        if is_viewable:
            material_index = mesh_faces[0].material_index
            material = mesh.materials[material_index]
            material_name = material.name
            if not material.name in our_materials:
                diffuse = OrderedDict(
                    [('r',material.diffuse_color[0]),
                     ('g',material.diffuse_color[1]),
                     ('b',material.diffuse_color[2]),
                     ('textures',[])]
                )
                if len(material.texture_slots):
                    material_texture = material.texture_slots[0]
                    if material_texture:
                        texture = material.texture_slots[0].texture
                        if texture.type == 'IMAGE':
                            diffuse['textures'].append(
                                make_texture_path(resources['texture_path'], texture.image.filepath)
                            )

                specular = OrderedDict(
                    [('r',material.specular_color[0]),
                     ('g',material.specular_color[1]),
                     ('b',material.specular_color[2]),
                     ('power',material.specular_hardness),
                     ('intensity',material.specular_intensity)]
                )
                material = OrderedDict(
                    [('diffuse',diffuse),
                    ('specular',specular)]
                )
                our_materials[material_name] = material

        # Collect the vertices for our mesh element
        # It's likely that we'll have 'duplicate' vertices when splitting our main
        # mesh into elements per material: duplicate in position and normal, but having
        # different UVs.
        our_vertices = {}   # index -> Vertex
        our_faces = []
        our_vertex_id = 0

        # iterate through all faces for those matching our selected material
        # for faces belonging to the same MeshElement, these faces are removed from
        # the mesh_faces list, and thus mesh_faces is meant to contain currently
        # unassociated faces
        faces = list(mesh_faces)

        for face in faces:
            # Then filter out unusable faces - i.e. points, lines, etc
            # also assert that our face has three and only three valid vertices
            # we're going to rely on the exporter to triangulate their meshes either
            # explicitly or via modifiers (which were applied above.)
            #
            if is_face_degenerate(face):
                print("OVEngineExport : found degenerate face")
                mesh_faces.remove(face)
                continue
            elif len(face.vertices) != 3:
                raise RuntimeError("Faces must be triangulated with vertex count = " + str(len(face.vertices)))
            elif is_viewable and face.material_index != material_index:
                continue
            # This face belongs to the current MeshElement
            mesh_faces.remove(face)

            face_vertices = []
            face_index = len(our_faces)
            for index in range(len(face.vertices)):
                vindex = face.vertices[index]
                if vindex not in our_vertices:
                    v = Vertex(id=our_vertex_id, pos=mesh_vertices[vindex].co,
                               normal=mesh_vertices[vindex].normal,
                               weights=[],
                               uv=[],
                               subvertices=[])
                    our_vertices[vindex] = v
                    our_vertex_id += 1

                    if has_weights:
                        # Handle bone weights if any. we find the bones by name, which
                        # is always the vertex group name and convert that to an index
                        for vgroup in mesh_vertices[vindex].groups:
                            vgroup_name = obj.vertex_groups[vgroup.group].name
                            bidx = [i for i,bone_name in enumerate(resources['_armature']['bone_names']) if bone_name == vgroup_name]
                            if not bidx:
                                raise RuntimeError("Bone["+vgroup_name+"] was not found in armature?")

                            v.weights.append((bidx[0], vgroup.weight))
                else:
                    v = our_vertices[vindex]

                # Handle Vertex with UV and possible creation of new vertices if necessary
                # See below
                if face_has_uvs:
                    face_uv = mesh_uv_textures.active.data[face.index].uv[index]
                    uv = [face_uv[0], face_uv[1]]
                    if uv != v.uv:
                        # mesh vertices may have the same position, normal but multiple
                        # uvs for the same texture.  shaders require single uv per vertex
                        # per sampler.  to resolve, duplicate the vertex so that this
                        # face will reference the new vertex.  we keep track of duplicates
                        # to construct the final mesh element
                        assign_uvs = True
                        if v.uv:
                            found = [subvertex for subvertex in v.subvertices if subvertex.uv==uv]
                            if found:
                                if len(found) > 1:
                                    raise RuntimeError("Two or more subvertices have the same UV")
                                v = found[0]
                                assign_uvs = False
                            else:
                                subvertex = Vertex(id=our_vertex_id,
                                                   pos=v.pos, normal=v.normal,
                                                   weights=v.weights,
                                                   uv=[],
                                                   subvertices=[])
                                v.subvertices.append(subvertex)
                                our_vertex_id += 1
                                v = subvertex

                        if assign_uvs:
                            v.uv.append(uv[0])
                            v.uv.append(uv[1])

                face_vertices.append(v)

            our_faces.append(face_vertices)

        #   Generate our final mesh_element dictionary
        our_mesh = OrderedDict()
        our_mesh['name'] = mesh_name + '.' + str(len(our_mesh_elements))
        if is_viewable:
            our_mesh['material'] = material_name
        our_mesh['vertices'] = []
        if is_viewable:
            our_mesh['normals'] = []
        if face_has_uvs:
            our_mesh['tex0'] = []
        if has_weights:
            our_mesh['weights'] = []
        our_mesh['tris'] = []

        # remap our face vertex list to vertex "IDs", which are really indices
        # into the our_mesh['vertices'] list.  *that* list is generated from the
        # new our_vertices dictionary generated below
        our_vertices = {}       # our_faces still keeps references to vertices

        for face_vertices in our_faces:
            for index in range(len(face_vertices)):
                vertex = face_vertices[index]
                if vertex.id not in our_vertices:
                    our_vertices[vertex.id] = vertex
                face_vertices[index] = vertex.id
            our_mesh['tris'].append(face_vertices)

        our_vertex_indices = sorted(our_vertices.keys())
        for vindex in our_vertex_indices:
            pos =  our_vertices[vindex].pos
            norm =  our_vertices[vindex].normal
            uv = our_vertices[vindex].uv
            weights = our_vertices[vindex].weights
            our_mesh['vertices'].append(OrderedDict([('x',pos[0]),('y',pos[1]),('z',pos[2])]))
            if 'normals' in our_mesh:
                our_mesh['normals'].append(OrderedDict([('x',norm[0]),('y',norm[1]),('z',norm[2])]))
            if 'tex0' in our_mesh:
                our_mesh['tex0'].append(OrderedDict([('u',uv[0]),('v',uv[1])]))
            if weights:
                bone_weights = []
                for w in weights:
                    bone_weights.append(OrderedDict([('bidx',w[0]),('weight',w[1])]))
                our_mesh['weights'].append(bone_weights)

        our_mesh_elements.append(our_mesh)

    return our_mesh_elements

def transform_obj_mtx_lh(obj):
    return mathutils.Matrix.Scale(-1,4,(0,0,1)) * obj.matrix_world

def transform_axis_mtx(y_up):
    convert_mtx = mathutils.Matrix.Identity(4)
    if y_up:
        return mathutils.Matrix.Rotation(math.radians(90.0),4,'X') * convert_mtx
    else:
        return convert_mtx

class ExportOVObjectJSON(bpy.types.Operator):
    """Export to OVEngine Objects (.json)"""
    bl_idname = "export.ovengine_objson"
    bl_label = "Export to OVEngine Objects"
    bl_options = { 'REGISTER' }

    modifier_options = (
        ('None','None',''),
        ('View','View',''),
        ('Render','Render','')
    )

    export_options = (
        ('Everything', 'Everything', ''),
        ('Meshes', 'Meshes', ''),
        ('Materials', 'Materials', ''),
        ('Animations', 'Animations', '')
    )

    filepath = bpy.props.StringProperty(subtype='FILE_PATH')

    opt_export_scene = bpy.props.BoolProperty(name="Export Scene")
    opt_export_y_up = bpy.props.BoolProperty(name="Export Root Objects with Y-Up", default=True)

    apply_modifiers = bpy.props.EnumProperty(name="Apply Modifiers", default="View",
                                             items=modifier_options)
    opt_export_type = bpy.props.EnumProperty(name='Export', default='Everything',
                                             items=export_options)

    # Animation framerates:
    #   scene_fps - Is the animation framerate from the scene's perspective.
    #               For example, the scene may assume that 24 frames = 1 second
    #               of animation.
    #
    #   sample_fps - Is the sampling frame rate for animation. When equal to scene_fps,
    #                the animation sampling during export will evaluate a keyframe per
    #                scene frame.  If greater than scene_fps, then sampling occurs at
    #                a rate greater than the scene framerate (for smoother animations)
    #                If less than the scene_fps, then sampling occurs at a lower rate
    #                than the scene framerate - which might be useful for reducing the
    #                number of keyframes per animation - though results in inaccurate
    #                animations.
    #                Try to keep scene_fps == sample_fps unless you know what you're
    #                doing.
    anim_fps = 24.0
    anim_sample_fps = 24.0

    kf_fields = {
        'location': ['tx','ty','tz'],
        'rotation_quaternion' : ['qw','qx','qy','qz'],
        'scale' : ['sx','sy','sz']
    }

    def exportObjectAsLight(self, scene, obj, resources):

        light_data = bpy.data.lamps[obj.name]

        light = OrderedDict()
        if light_data.type == 'SPOT':
            light['type'] = 'spot'
        elif light_data.type == 'SUN':
            light['type'] = 'directional'
        elif light_data.type == 'POINT':
            light['type'] = 'point'
        elif light_data.type == 'HEMI':
            light['type'] = 'ambient'
        else:
            self.report({'WARN'}, obj.name + ": Unsupported light type " + light_data.type)
            return -1

        light['color'] = OrderedDict(
                [('r',light_data.color[0]),
                 ('g',light_data.color[1]),
                 ('b',light_data.color[2])])
        light['intensity'] = light_data.energy

        if light_data.type == 'SPOT' or light_data.type == 'POINT':
            light['distance'] = light_data.distance
            # default to constant
            l = 0.0
            q = 0.0
            if light_data.falloff_type == 'LINEAR_QUADRATIC_WEIGHTED':
                l = light_data.linear_attenuation
                q = light_data.quadratic_attenuation
            elif light_data.falloff_type == 'INVERSE_LINEAR':
                l = 1.0
                q = 0.0
            elif light_data.falloff_type == 'INVERSE_SQUARE':
                l = 1.0
                q = 1.0
            else:
                self.report({'WARN'}, obj.name + ": Unsupported light falloff " + light_data.falloff_type)

            light['falloff'] = {
                'l': light_data.linear_attenuation,
                'q': light_data.quadratic_attenuation
            }

        if light_data.type == 'SPOT':
            light['cutoff'] = light_data.spot_size

        resources['lights'].append(light)

        return len(resources['lights'])-1

    def exportObjectAsMesh(self, scene, obj, resources, type):
        # Process the selected mesh
        apply_modifiers = False
        export_settings = 'PREVIEW'
        if self.apply_modifiers != 'None':
            apply_modifiers = True
            if self.apply_modifiers == 'Render':
                export_settings = 'RENDER'

        object_name = obj.name

        print("OVEngineExport : exporting object ", object_name,
              " with modifiers: ", self.apply_modifiers)

        object_mesh = obj.to_mesh(scene, apply_modifiers, export_settings, calc_tessface=True)
        mesh_indices = []

        try:
            our_meshes = generate_mesh_elements(obj, object_mesh, resources)

            if type == 'Hull':
                hull_index = len(resources['hulls'])
                for hull in our_meshes:
                    resources['hulls'].append(hull)
                    mesh_indices.append(hull_index)
                    hull_index += 1
            else:
                mesh_index = len(resources['meshes'])
                for mesh in our_meshes:
                    resources['meshes'].append(mesh)
                    mesh_indices.append(mesh_index)
                    mesh_index += 1

        except Exception as e:
            self.report({'ERROR'}, str(e))

        bpy.data.meshes.remove(object_mesh)

        return mesh_indices


    def exportArmatureAction(self, obj, action, resources):
        # print(" Name ", action.name)

        """
        An armature animation consists of one or more bone animations identified by
        bone name.

        Each bone animation consists of position, orientation and scale keyframe lists.
        Each keyframe is a time, value pair.

        animation = {
            "bone.001": {
                <tx|ty|tz|qw|qx|qy|qz|sx|sy|sz>: [
                    {
                        't': <ticks>,
                        'v': <value>
                    },
                    ...
                ],
            },
            "bone.002": {
                ...
            },
            ...
        }
        """
        animation = OrderedDict()
        sample_rate = self.anim_fps / self.anim_sample_fps
        for fcurve in action.fcurves:
            start,end = fcurve.range()
            kfpoints = fcurve.keyframe_points

            # decipher property this fcurve manipulates - we only care about pose bones for
            # armature animations
            data_root, data_loc = fcurve.data_path.split('.',1)
            if data_root != 'pose':
                self.report({'WARN'}, "Action '"+action.name+"' fcurve does not point to an armature.")
                continue

            bone_name, bone_prop = data_loc.split('[', 1)[1].split(']',1)
            bone_name = bone_name.strip("'\"")
            prop_name = bone_prop[1:]
            prop_field = fcurve.array_index
            if bone_name not in animation:
                animation[bone_name] = OrderedDict()
            bone_animation = animation[bone_name]

            # add keyframe sequence to bone animation if used
            kf_props = None
            if prop_name in self.kf_fields:
                kf_props = self.kf_fields[prop_name]
                if kf_props[prop_field] not in bone_animation:
                    bone_animation[kf_props[prop_field]] = []
            else:
                self.report({'WARN'}, "Action '"+action.name+"' fcurve has an unused property '"+prop_name+"'")

            if kf_props:
                kf_sequence = bone_animation[kf_props[prop_field]]
                last_kf = None
                for kf in kfpoints:
                    if last_kf is not None and last_kf.co[1] != kf.co[1]:
                        # interpolation pass for keyframes with different values only
                        # this is to reduce the number of animation samples generated
                        v_scalar = 1.0
                        if kf_props[prop_field] in ['qx','qy','qz']:
                            v_scalar = -1.0

                        t = last_kf.co[0]
                        while t < kf.co[0]:
                            kf_sequence.append(OrderedDict([
                                ('t', t/self.anim_fps),
                                ('v', v_scalar * fcurve.evaluate(t))
                            ]))
                            t += sample_rate

                    last_kf = kf

                # keyframe sequence that doesn't change - store at least key frame for
                # reference
                if not kf_sequence:
                    kf_sequence.append(OrderedDict([
                        ('t',kfpoints[0].co[0]/self.anim_fps),
                        ('v',kfpoints[0].co[1])
                    ]))

        return animation


    def exportObjectAsArmature(self, scene, obj, resources):
        # each bone requires its own offset matrix based on a common space
        # (relative to the root so we can store every bone in a list
        # instead of a hierarchy)
        # generate the bone list, indices are used as bone IDs for bone ID:weight
        # pairing
        bones = []
        bone_names = []

        def add_bone(bone, adj_matrix=mathutils.Matrix.Identity(4)):
            offset = adj_matrix * bone.matrix_local
            node = OrderedDict([
                ('name', bone.name),
                ('matrix', matrix_to_list(offset)),
                ('bone_index', len(bone_names)),
                ('children', [])
            ])
            bone_names.append(bone.name)
            for b in bone.children:
                node['children'].append(add_bone(b))

            return node

        for bone in obj.data.bones:
            if not bone.parent:
                bones.append(add_bone(bone))

        animations = {}

        if not obj.animation_data.use_nla:
            print("Use Active Action")
            action = obj.animation_data.action
            animations[action.name] = self.exportArmatureAction(obj, action, resources)
        else:
            print("Use NLA (All actions derived from strips)")
            init_action = obj.animation_data.action
            current_frame = scene.frame_current

            for track in obj.animation_data.nla_tracks:
                for strip in track.strips:
                    action = strip.action
                    obj.animation_data.action = action
                    animations[action.name] = self.exportArmatureAction(obj, action, resources)

            obj.animation_data.action = init_action
            scene.frame_set(current_frame)

        return { 'skeleton': bones, 'bone_names': bone_names, 'animations': animations }

    def createNode(self, obj, matrix):
        node = OrderedDict()
        if type(obj) is str:
            node['name'] = obj
            node['type'] = 'Node'
        else:
            node['name'] = obj.name
            if hasattr(obj, 'ovobject_type_property'):
                node['type'] = obj.ovobject_type_property
            else:
                node['type'] = 'Node'

        node['matrix'] = matrix_to_list(matrix)
        node['obb'] = OrderedDict()
        node['obb']['min'] = OrderedDict([('x', 0.0),('y', 0.0), ('z', 0.0)])
        node['obb']['max'] = OrderedDict([('x', 0.0),('y', 0.0), ('z', 0.0)])
        return node

    def exportNode(self, scene, obj, resources, matrix):
        skip_object = obj.hide_render
        if obj.type == 'CAMERA':
            skip_object = True

        if skip_object:
            print("Skipping [",obj.name,"] of type ",obj.type)
            return None

        node = self.createNode(obj, matrix)

        min = node['obb']['min']
        max = node['obb']['max']

        for vx,vy,vz in obj.bound_box:
            if vx < min['x']:
                min['x'] = vx
            if vy < min['y']:
                min['y'] = vy
            if vz < min['z']:
                min['z'] = vz
            if vx > max['x']:
                max['x'] = vx
            if vy > max['y']:
                max['y'] = vy
            if vz > max['z']:
                max['z'] = vz

        node['obb']['min'] = min
        node['obb']['max'] = max

        if obj.type == 'LAMP':
            node['light'] = self.exportObjectAsLight(scene, obj, resources)
        elif obj.type == 'ARMATURE':
            # persist bone list, which may be used when generating vertex weights from the
            # underlying meshes.  we free this context after processing the armature's
            # hierarchy
            resources['_armature'] = self.exportObjectAsArmature(scene, obj, resources)
            resources['animations'][obj.name] = {
                'states': resources['_armature']['animations'],
                'skeleton' : resources['_armature']['skeleton']
            }
            node['animation'] = obj.name
        elif obj.type == 'MESH':
            if node['type'] == 'Hull' or node['type'] == 'Entity':
                node['hulls'] = self.exportObjectAsMesh(scene, obj, resources, node['type'])
            else:
                node['meshes'] = self.exportObjectAsMesh(scene, obj, resources, node['type'])

        # Seems hacky.  But the only way I found to determine if an object has custom
        # props is to check for the _RNA_UI key
        obj_prop_keys = obj.keys()
        if '_RNA_UI' in obj_prop_keys and len(obj_prop_keys) > 1:
            custom_props = {}
            for custom_prop_key in obj_prop_keys:
                if custom_prop_key != '_RNA_UI':
                    custom_props[custom_prop_key] = obj[custom_prop_key]
            node['properties'] = custom_props

        if obj.children:
            node['children'] = []
            for child in obj.children:
                child_node = self.exportNode(scene, child, resources, matrix=child.matrix_local)
                if child_node:
                    node['children'].append(child_node)

        # cleanup
        if obj.type == 'ARMATURE':
            del resources["_armature"]

        return node

    def execute(self, context):
        target_filepath = bpy.path.ensure_ext(self.filepath, ".json")

        resources = {
            'materials' : {},
            'animations': {},
            'lights' : [],
            'meshes' : [],
            'hulls' : [],
            'texture_path' : 'textures'
        }

        def exportObject(scene, obj):
            world_matrix = transform_axis_mtx(self.opt_export_y_up) * transform_obj_mtx_lh(obj)
            return self.exportNode(scene, obj, resources, matrix=world_matrix)

        exported_objects = None
        if self.opt_export_scene:
            exported_objects = scene.objects
        else:
            exported_objects = bpy.context.selected_objects

        root = None
        scene = bpy.context.scene
        root = self.createNode('root', mathutils.Matrix.Identity(4))
        root['children'] = []
        for obj in exported_objects:
            if not obj.parent:
                child = exportObject(scene, obj)
                if child:
                    root['children'].append(child)

        document = OrderedDict()

        if self.opt_export_type == 'Everything' or self.opt_export_type == 'Materials':
            document['materials'] = resources['materials']
        if self.opt_export_type == 'Everything' or self.opt_export_type == 'Animations':
            document['animations'] = resources['animations']
        if self.opt_export_type == 'Everything' or self.opt_export_type == 'Meshes':
            document['meshes'] = resources['meshes']
            document['hulls'] = resources['hulls']
            document['lights'] = resources['lights']
            document['nodes'] = root

        try:
            f = open(target_filepath, "w")
            json.dump(document, f, indent=2)
            f.close()
        except Exception as e:
            raise e

        return { 'FINISHED' }

    def invoke(self, context, event):
        if not self.filepath:
            self.filepath = bpy.path.ensure_ext(bpy.data.filepath, ".json")
        context.window_manager.fileselect_add(self)
        return { 'RUNNING_MODAL' }

class OVObjectPanel(bpy.types.Panel):
    bl_label = "OVEngine Properties"
    bl_space_type = "PROPERTIES"
    bl_region_type = "WINDOW"
    bl_context = "object"

    type_options = (
        ('Node', 'Node', ''),
        ('Entity', 'Entity', ''),
        ('View', 'View', ''),
        ('Hull', 'Hull', ''),
        ('Model', 'Model', '')
    )
    bpy.types.Object.ovobject_type_property = bpy.props.EnumProperty(name='Type', default='Node',
                                                    items=type_options)

    def draw(self, context):
        self.layout.operator("prop.ovobject_debug", text='Debug')
        self.layout.prop(context.active_object,"ovobject_type_property")

class OBJECT_OT_PropCopyButton(bpy.types.Operator):
    bl_idname = "prop.ovobject_debug"
    bl_label = "OVEngine Debug"

    def execute(self, context):
        sc0 = context.active_object
        print("OVObject Property : ", sc0.ovobject_type_property)
        return{'FINISHED'}

def menu_func(self, context):
    self.layout.operator_context = 'INVOKE_DEFAULT'
    self.layout.operator(ExportOVObjectJSON.bl_idname, text="OVEngine Objects (.json)")

def register():
    bpy.utils.register_module(__name__)
    bpy.types.INFO_MT_file_export.append(menu_func)

def unregister():
    bpy.types.INFO_MT_file_export.remove(menu_func)
    bpy.utils.unregister_module(__name__)


#if __name__ == "__main__":
#    register()
#    bpy.ops.export.ovengine_objson()


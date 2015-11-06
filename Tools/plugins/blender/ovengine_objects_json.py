bl_info = {
    "name" : "Export OVEngine Objects (.json)",
    "author" : "Samir Sinha",
    "version" : (0,0,1),
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

    while mesh_faces:
        # Subdivide the Mesh into MeshElements (An element being a mesh
        # with a single common material)
        #
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
            if material.texture_slots:
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
            elif face.material_index != material_index:
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
        our_mesh['material'] = material_name
        our_mesh['vertices'] = []
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
            our_mesh['normals'].append(OrderedDict([('x',norm[0]),('y',norm[1]),('z',norm[2])]))
            our_mesh['tex0'].append(OrderedDict([('u',uv[0]),('v',uv[1])]))
            if weights:
                bone_weights = []
                for w in weights:
                    bone_weights.append(OrderedDict([('bidx',w[0]),('weight',w[1])]))
                our_mesh['weights'].append(bone_weights)

        our_mesh_elements.append(our_mesh)

    return our_mesh_elements

def convert_matrix(y_up):
    lh_convert_mtx = mathutils.Matrix.Scale(-1,4,(0,0,1))
    if y_up:
        return mathutils.Matrix.Rotation(math.radians(90.0),4,'X') * lh_convert_mtx
    else:
        return lh_convert_mtx

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

    filepath = bpy.props.StringProperty(subtype='FILE_PATH')
    apply_modifiers = bpy.props.EnumProperty(name="Apply Modifiers", default="View",
                                             items=modifier_options)
    opt_export_scene = bpy.props.BoolProperty(name="Export Scene")
    opt_export_y_up = bpy.props.BoolProperty(name="Export Root Objects with Y-Up")

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

    def exportObjectAsMesh(self, scene, obj, resources):
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

    def createNode(self, name, matrix):
        node = OrderedDict()
        node['name'] = name
        node['matrix'] = matrix_to_list(matrix)
        node['obb'] = OrderedDict()
        node['obb']['min'] = OrderedDict([('x', 0.0),('y', 0.0), ('z', 0.0)])
        node['obb']['max'] = OrderedDict([('x', 0.0),('y', 0.0), ('z', 0.0)])
        return node

    def exportNode(self, scene, obj, resources, matrix):
        skip_object = False
        if obj.type == 'CAMERA' or obj.type == 'LAMP':
            skip_object = True

        if skip_object:
            print("Skipping camera [",obj.name,"] of type ",obj.type)
            return None

        node = self.createNode(obj.name, matrix)

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

        if obj.type == 'ARMATURE':
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
            node['meshes'] = self.exportObjectAsMesh(scene, obj, resources)

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
            'meshes' : [],
            'texture_path' : 'textures'
        }

        root = None
        scene = bpy.context.scene
        if self.opt_export_scene:
            root = self.createNode('root', mathutils.Matrix.Identity(4))
            root['children'] = []
            for obj in scene.objects:
                if not obj.parent:
                    world_matrix = convert_matrix(self.opt_export_y_up) * obj.matrix_world
                    child = self.exportNode(scene, obj, resources, matrix=world_matrix)
                    if child:
                        root['children'].append(child)
        else:
            obj = scene.objects.active
            world_matrix = convert_matrix(self.opt_export_y_up) * obj.matrix_world
            root = self.exportNode(scene, obj, resources, matrix=world_matrix)

        document = OrderedDict(
            [('materials', resources['materials']),
             ('animations', resources['animations']),
             ('meshes', resources['meshes']),
             ('nodes', root)]
        )

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


def menu_func(self, context):
    self.layout.operator_context = 'INVOKE_DEFAULT'
    self.layout.operator(ExportOVObjectJSON.bl_idname, text="OVEngine Objects (.json)")

def register():
    bpy.utils.register_module(__name__);
    bpy.types.INFO_MT_file_export.append(menu_func)

def unregister():
    bpy.types.INFO_MT_file_export.remove(menu_func)
    bpy.utils.unregister_module(__name__);


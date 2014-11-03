#!/usr/bin/env node

var fs = require('fs');
var prog = require('commander');
var setcase = require('change-case');

// TODO 
// separar en actions/functions
// creación de components y de sistemas
// pasar lista de componentes a make --system
// manteniendo la creación de un componente y su sistema con el mismo comando (el sistema tiene ahí solo ese componente por default)
// agregar systems on the fly en ofApp

// ./make.js mi_xxx_cs

prog
    .option('-c, --component [name_with_snake_case]', 'Add the specified component')
    .option('-s, --system', 'Flag to also add system for the specified component with -c')
    .option('-r, --remove [name_with_snake_case]', 'Remove the specified component')
    .parse(process.argv);

var comp_name = prog.remove || prog.component;

if ( !comp_name ) {
    console.log('error: component name is',comp_name);
    return;
}

var comp_pascalcase = setcase.pascalCase(comp_name);
var comp_snakecase = setcase.snakeCase(comp_name);

console.log(' - make component name:', comp_name);
console.log(' - pascal case:', comp_pascalcase);
console.log(' - snake case:', comp_snakecase);

// files

var comp_filename = comp_pascalcase+'Component.h';

console.log(' - file name:', comp_filename);

var comp_path = __dirname + '/src/components/'+ comp_filename;
var components_file_path = __dirname + '/src/components/Components.h';
var comp_include = '#include "components/'+ comp_pascalcase +'Component.h"';

var comp_factory_path = __dirname + '/src/components/ComponentFactory.h'; 
var comp_factory_code = 'else if (id == "'+ comp_snakecase +'") return new '+ comp_pascalcase +'Component(id);';


var sys_filename = comp_pascalcase+'System.h';
var sys_path = __dirname + '/src/systems/'+ sys_filename;
var systems_file_path = __dirname + '/src/systems/Systems.h';
var sys_include = '#include "systems/'+ comp_pascalcase +'System.h"';

var sys_factory_path = __dirname + '/src/ecs/add_systems.cpp'; 
var sys_factory_code = 'add_system(new '+ comp_pascalcase +'System("'+comp_snakecase+'"));'


//---Remove Component

if ( prog.remove !== undefined ) {

    //remove files

    //component
    if (fs.existsSync(comp_path)) {
        fs.unlink( comp_path, function (err) {
            if (err) throw err;
            console.log('successfully deleted',comp_path);
        });
    }

    //system
    if (fs.existsSync(sys_path)) {
        fs.unlink( sys_path, function (err) {
            if (err) throw err;
            console.log('successfully deleted',sys_path);
        });
    }

    //remove includes

    //component
    if (fs.existsSync(components_file_path)) {
        fs.readFile( components_file_path, function(err, data) {
            if (err) throw err;
            if (data.toString().indexOf( comp_include ) == -1 ) {
                console.log('no includes found on',components_file_path);
                return;
            }
            var newdata = data.toString().replace(comp_include,'');

            fs.writeFile( components_file_path, newdata, function(err) { 
                if (err) throw err;
                console.log('removed line for component include in',components_file_path);
            });
        });
    }

    //system
    if (fs.existsSync(systems_file_path)) {
        fs.readFile( systems_file_path, function(err, data) {
            if (err) throw err;
            if (data.toString().indexOf( sys_include ) == -1 ) {
                console.log('no includes found on',systems_file_path);
                return;
            }
            var newdata = data.toString().replace(sys_include,'');

            fs.writeFile( systems_file_path, newdata, function(err) { 
                if (err) throw err;
                console.log('removed line for system include in',systems_file_path);
            });
        });
    }

    //remove component from factory
    fs.readFile( comp_factory_path, function(err, data) {
        if (err) throw err;
        if (data.toString().indexOf( comp_factory_code ) == -1 ) {
            console.log('no factory found on',comp_factory_path);
            return;
        }
        var newdata = data.toString().replace(comp_factory_code + '\n\t\t','');

        fs.writeFile( comp_factory_path, newdata, function(err) { 
            if (err) throw err;
            console.log('removed line for component factory in',comp_factory_path);
        });
    });

    //remove system from factory
    fs.readFile( sys_factory_path, function(err, data) {
        if (err) throw err;
        if (data.toString().indexOf( sys_factory_code ) == -1 ) {
            console.log('no factory found on',sys_factory_path);
            return;
        }
        var newdata = data.toString().replace(sys_factory_code + '\n\t','');

        fs.writeFile( sys_factory_path, newdata, function(err) { 
            if (err) throw err;
            console.log('removed line for system factory in',sys_factory_path);
        });
    });

    return;
}

//---Add Component

// copiar ./src/components/TemplateComponent.h a ./src/components/MiXxxCsComponent.h
// en MiXxxCsComponent.h reemplazar:
// * TemplateComponent por MiXxxCsComponent

fs.readFile( __dirname + '/src/components/TemplateComponent.h', function (err, data) {
    if (err) throw err;

    if (fs.existsSync(comp_path)) {
        console.log('warning:',comp_path,'already exists');
        return;
    }

    var comp_filecontent = data.toString().replace( /Template/g, comp_pascalcase);

    fs.writeFile( comp_path, comp_filecontent, function (err) {
        if (err) throw err;
        console.log(comp_path,'made succesfuly');
    });

});

// agregar componente a Components.h

fs.readFile( components_file_path, function (err, data) {
    if (err) throw err;

    if ( data.toString().indexOf( comp_include ) > -1 ) {
        console.log('warning: component for',comp_name,'already included in',components_file_path);
        return;
    }

    fs.appendFile( components_file_path, comp_include, function (err) {
        if (err) throw err;
        console.log('component',comp_name,'included in',components_file_path);
    });

});

// agregar componente a ComponentFactory.h

fs.readFile( comp_factory_path, function (err, data) {
    if (err) throw err;

    if ( data.toString().indexOf( comp_factory_code ) > -1 ) {
        console.log('warning: component factory code for',comp_name,'already included in',comp_factory_path);
        return;
    }

    var placeholder = '//dont remove this line';
    var newdata = data.toString().replace(placeholder, comp_factory_code + '\n\t\t' + placeholder);

    fs.writeFile( comp_factory_path, newdata, function(err) { 
        if (err) throw err;
        console.log('factory code for component',comp_name,'added in',comp_factory_path);
    });

});


if ( !prog.system ) return;

//---Add System

// copiar ./src/systems/TemplateSystem.h a ./src/systems/MiXxxCsSystem.h
// en MiXxxCsSystem.h reemplazar:
// * TemplateComponent por MiXxxCsComponent
// * TemplateSystem por MiXxxCsSystem
// * template_m por mi_xxx_cs_m

fs.readFile( __dirname + '/src/systems/TemplateSystem.h', function (err, data) {
    if (err) throw err; 

    if (fs.existsSync(sys_path)) {
        console.log('warning:',sys_path,'already exists');
        return;
    }

    var sys_filecontent = data.toString().replace( /Template/g, comp_pascalcase).replace(/template/g, comp_snakecase);

    fs.writeFile( sys_path, sys_filecontent, function (err) {
        if (err) throw err;
        console.log(sys_path,'made succesfuly');
    });

});

// agregar sistema a Systems.h

fs.readFile( systems_file_path, function (err, data) {
    if (err) throw err;

    if ( data.toString().indexOf( sys_include ) > -1 ) {
        console.log('warning: system for',comp_name,'already included in',systems_file_path);
        return;
    }

    fs.appendFile( systems_file_path, sys_include, function (err) {
        if (err) throw err;
        console.log('system',comp_name,'included in',systems_file_path);
    });

});

// agregar sistema a add_systems.h

fs.readFile( sys_factory_path, function (err, data) {
    if (err) throw err;

    if ( data.toString().indexOf( sys_factory_code ) > -1 ) {
        console.log('warning: system factory code for',comp_name,'already included in',sys_factory_path);
        return;
    }

    var placeholder = '//dont remove this line';
    var newdata = data.toString().replace(placeholder, sys_factory_code + '\n\t' + placeholder);

    fs.writeFile( sys_factory_path, newdata, function(err) { 
        if (err) throw err;
        console.log('factory code for system',comp_name,'added in',sys_factory_path);
    });

});


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

// ./ecs mi_xxx_cs

prog
    .option('-n, --name [name_with_snake_case]', 'Name for the specified component and/or system')
    .option('-a, --add', 'Add system and/or component')
    .option('-r, --remove', 'Remove system and/or component')
    .option('-c, --component', 'Process a component')
    .option('-s, --system', 'Process a system')
    .parse(process.argv);

if ( prog.add === undefined && prog.remove === undefined ) {
    console.log('error: pick --add OR --remove / -h for help');
    return;
}

if ( prog.add !== undefined && prog.remove !== undefined ) {
    console.log('error: pick --add OR --remove not both / -h for help');
    return;
}

if ( prog.component === undefined && prog.system === undefined ) {
    console.log('error: pick --component AND/OR --system / -h for help');
    return;
}

var comp_name = prog.name;

if ( typeof comp_name !== 'string' ) {
    console.log('error: --name is',comp_name,' / -h for help');
    return;
}

var placeholder = '//dont remove this line';

var comp_pascalcase = setcase.pascalCase(comp_name);
var comp_snakecase = setcase.snakeCase(comp_name);

console.log(' - component name:', comp_name);
console.log(' - pascal case:', comp_pascalcase);
console.log(' - snake case:', comp_snakecase);

// files

var comp_filename = comp_pascalcase+'Component.h';
var comp_path = __dirname + '/src/components/'+ comp_filename;

var comp_include_path = __dirname + '/src/components/Components.h';
var comp_include_code = '#include "components/'+ comp_pascalcase +'Component.h"' + '\n';

var comp_factory_path = __dirname + '/src/components/PlabComponentFactory.h'; 
var comp_factory_code = 'else if (id == "'+ comp_snakecase +'") return new '+ comp_pascalcase +'Component(id);' + '\n\t\t';


var sys_filename = comp_pascalcase+'System.h';
var sys_path = __dirname + '/src/systems/'+ sys_filename;

var sys_include_path = __dirname + '/src/systems/Systems.h';
var sys_include_code = '#include "systems/'+ comp_pascalcase +'System.h"' + '\n';

var sys_factory_path = __dirname + '/src/systems/PlabSystemFactory.h'; 
var sys_factory_code = 'ecs.add_system(new '+ comp_pascalcase +'System("'+comp_snakecase+'"));' + '\n\t\t';


//Remove

if ( prog.remove !== undefined ) {

    if ( prog.component !== undefined ) {

        //remove files
        if (fs.existsSync(comp_path)) {
            fs.unlink( comp_path, function (err) {
                if (err) throw err;
                console.log('successfully deleted',comp_path);
            });
        }

        //remove includes
        if (fs.existsSync(comp_include_path)) {
            fs.readFile( comp_include_path, function(err, data) {
                if (err) throw err;
                if (data.toString().indexOf( comp_include_code ) == -1 ) {
                    console.log('no includes found on',comp_include_path);
                    return;
                }
                var newdata = data.toString().replace(comp_include_code,'');

                fs.writeFile( comp_include_path, newdata, function(err) { 
                    if (err) throw err;
                    console.log('removed line for component include in',comp_include_path);
                });
            });
        }

        //remove from factory
        fs.readFile( comp_factory_path, function(err, data) {
            if (err) throw err;
            if (data.toString().indexOf( comp_factory_code ) == -1 ) {
                console.log('no factory found on',comp_factory_path);
                return;
            }
            var newdata = data.toString().replace(comp_factory_code,'');

            fs.writeFile( comp_factory_path, newdata, function(err) { 
                if (err) throw err;
                console.log('removed line for component factory in',comp_factory_path);
            });
        });

    }

    if ( prog.system !== undefined ) {

        //remove files
        if (fs.existsSync(sys_path)) {
            fs.unlink( sys_path, function (err) {
                if (err) throw err;
                console.log('successfully deleted',sys_path);
            });
        }

        //remove includes
        if (fs.existsSync(sys_include_path)) {
            fs.readFile( sys_include_path, function(err, data) {
                if (err) throw err;
                if (data.toString().indexOf( sys_include_code ) == -1 ) {
                    console.log('no includes found on',sys_include_path);
                    return;
                }
                var newdata = data.toString().replace(sys_include_code,'');

                fs.writeFile( sys_include_path, newdata, function(err) { 
                    if (err) throw err;
                    console.log('removed line for system include in',sys_include_path);
                });
            });
        }

        //remove from factory
        fs.readFile( sys_factory_path, function(err, data) {
            if (err) throw err;
            if (data.toString().indexOf( sys_factory_code ) == -1 ) {
                console.log('no factory found on',sys_factory_path);
                return;
            }
            var newdata = data.toString().replace(sys_factory_code,'');

            fs.writeFile( sys_factory_path, newdata, function(err) { 
                if (err) throw err;
                console.log('removed line for system factory in',sys_factory_path);
            });
        });
    }  

    return;
}

//Add

if ( prog.component !== undefined ) {

    // copia ./src/components/TemplateComponent.h a ./src/components/MiXxxCsComponent.h
    // en MiXxxCsComponent.h reemplaza: 
    // * TemplateComponent por MiXxxCsComponent

    //crear desde TemplateComponent

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

    // agregar a Components.h

    fs.readFile( comp_include_path, function (err, data) {
        if (err) throw err;

        if ( data.toString().indexOf( comp_include_code ) > -1 ) {
            console.log('warning: component for',comp_name,'already included in',comp_include_path);
            return;
        }

        var newdata = data.toString().replace(placeholder, comp_include_code + placeholder);

        fs.writeFile( comp_include_path, newdata, function(err) { 
            if (err) throw err;
            console.log('component',comp_name,'included in',comp_include_path);
        });

    });

    // agregar a xxxComponentFactory.h

    fs.readFile( comp_factory_path, function (err, data) {
        if (err) throw err;

        if ( data.toString().indexOf( comp_factory_code ) > -1 ) {
            console.log('warning: component factory code for',comp_name,'already included in',comp_factory_path);
            return;
        }

        var newdata = data.toString().replace(placeholder, comp_factory_code + placeholder);

        fs.writeFile( comp_factory_path, newdata, function(err) { 
            if (err) throw err;
            console.log('factory code for component',comp_name,'added in',comp_factory_path);
        });

    });
}


if ( prog.system !== undefined ) {

// copia ./src/systems/TemplateSystem.h a ./src/systems/MiXxxCsSystem.h
// en MiXxxCsSystem.h reemplaza:
// * TemplateComponent por MiXxxCsComponent
// * TemplateSystem por MiXxxCsSystem
// * template_m por mi_xxx_cs_m

    //crear desde TemplateSystem

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

    // agregar a Systems.h

    fs.readFile( sys_include_path, function (err, data) {
        if (err) throw err;

        if ( data.toString().indexOf( sys_include_code ) > -1 ) {
            console.log('warning: system for',comp_name,'already included in',sys_include_path);
            return;
        }

        var newdata = data.toString().replace(placeholder, sys_include_code + placeholder);

        fs.writeFile( sys_include_path, newdata, function(err) { 
            if (err) throw err;
            console.log('system',comp_name,'included in',sys_include_path);
        });

    });

    // agregar a xxxSystemFactory

    fs.readFile( sys_factory_path, function (err, data) {
        if (err) throw err;

        if ( data.toString().indexOf( sys_factory_code ) > -1 ) {
            console.log('warning: system factory code for',comp_name,'already included in',sys_factory_path);
            return;
        }

        var newdata = data.toString().replace(placeholder, sys_factory_code + placeholder);

        fs.writeFile( sys_factory_path, newdata, function(err) { 
            if (err) throw err;
            console.log('factory code for system',comp_name,'added in',sys_factory_path);
        });

    });
}


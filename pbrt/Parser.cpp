// ======================================================================== //
// Copyright 2015 Ingo Wald
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#include "parser.h"
// stl
#include <fstream>
#include <sstream>
#include <queue>
#include <stack>
// std
#include <stdio.h>
#include <string.h>

namespace plib {
  namespace pbrt {
    using namespace std;

    struct File : public RefCounted {
      File(const FileName &fn)
        : name(fn)
      {
        file = fopen(fn.str().c_str(),"r");
      }

      FileName name;
      FILE *file;
    };

    struct Loc {
      Loc(Ref<File> file) : file(file), line(1), col(0) {}
      Loc(const Loc &loc) : file(loc.file), line(loc.line), col(loc.col) {}
      
      std::string toString() const { 
        std::stringstream ss;
        ss << "@" << file->name.str() << ":" << line << "." << col;
        return ss.str();
      }

      Ref<File> file;
      int line, col;
    };

    struct Token : public RefCounted {
      static Ref<Token> TOKEN_EOF;

      typedef enum { TOKEN_TYPE_EOF, TOKEN_TYPE_STRING, TOKEN_TYPE_LITERAL, TOKEN_TYPE_SPECIAL } Type;

      Token(const Loc &loc, 
            const Type type,
            const std::string &text) 
        : loc(loc), type(type), text(text) 
      {}
      
      std::string toString() const { 
        std::stringstream ss;
        ss << loc.toString() <<": " << text;
        return ss.str();
      }
      const Loc         loc;
      const std::string text;
      const Type        type;
    };

    Ref<Token> Token::TOKEN_EOF = new Token(Loc(NULL),Token::TOKEN_TYPE_EOF,"<EOF>");

    struct Tokenizer {

      Tokenizer(const FileName &fn)
        : file(new File(fn)), loc(file), peekedChar(-1) 
      {
      }

      std::deque<Ref<Token> > peekedTokens;

      Loc getLastLoc() { return loc; }

      Ref<Token> peek(size_t i=0)
      {
        while (i >= peekedTokens.size())
          peekedTokens.push_back(produceNextToken());
        return peekedTokens[i];
      }
      
      void unget_char(int c)
      {
        if (peekedChar >= 0) 
          THROW_RUNTIME_ERROR("can't push back more than one char ...");
        peekedChar = c;
      }

      int get_char() 
      {
        if (peekedChar >= 0) {
          int c = peekedChar;
          peekedChar = -1;
          // PING; PRINT(c);
          return c;
        }
        
        if (!file->file || feof(file->file)) {
          // PING; 
          return -1;
        }
        
        int c = fgetc(file->file);
        int eol = '\n';
        // PRINT(eol);
        if (c == '\n') {
          // PING;
          // cout << "NEWLINE" << endl;
          loc.line++;
          loc.col = 0;
        } else {
          loc.col++;
        }
        // PING; PRINT(loc.toString()); PRINT(c); PRINT((char)c);
        return c;
      };
      
      inline bool isWhite(const char c)
      {
        return strchr(" \t\n\r",c)!=NULL;
      }
      inline bool isSpecial(const char c)
      {
        return strchr("[,]",c)!=NULL;
      }

      Ref<Token> next(bool consumePeeks=false) 
      {
        if (peekedTokens.empty())
          return produceNextToken();

        Ref<Token> token = peekedTokens.front();
        peekedTokens.pop_front();
        return token;
      }

      Ref<Token> produceNextToken() 
      {
        // skip all white space and comments
        // int c = peeked_char;
        // if (c < 0) return Token::TOKEN_EOF;
        int c;

        std::stringstream ss;

        Loc startLoc = loc;
        // PING;
        // skip all whitespaces and comments
        while (1) {
          // PING;
          c = get_char();
          // PRINT(c);
          // PRINT((char)c);

          if (c < 0) return Token::TOKEN_EOF;
          
          if (isWhite(c)) {
            // PING; PRINT((char)c); PRINT(loc.toString());
            continue;
          }
          
          if (c == '#') {
            startLoc = loc;
            Loc lastLoc = loc;
            // std::cout << "start of comment at " << startLoc.toString() << std::endl;
            while (c != '\n') {
              lastLoc = loc;
              c = get_char();
              if (c < 0) return Token::TOKEN_EOF;
            }
            // std::cout << "END of comment at " << lastLoc.toString() << std::endl;
            continue;
          }
          break;
        }

        // PING;
        // cout << "end of whitespace at " << loc.toString() << " c = " << (char)c << endl;
        // -------------------------------------------------------
        // STRING LITERAL
        // -------------------------------------------------------
        startLoc = loc;
        Loc lastLoc = loc;
        if (c == '"') {
          // cout << "START OF STRING at " << loc.toString() << endl;
          while (1) {
            lastLoc = loc;
            c = get_char();
            if (c < 0)
              THROW_RUNTIME_ERROR("could not find end of string literal (found eof instead)");
            if (c == '"') 
              break;
            ss << (char)c;
          } 
          return new Token(startLoc,Token::TOKEN_TYPE_STRING,ss.str());
        }

        // -------------------------------------------------------
        // special char
        // -------------------------------------------------------
        if (isSpecial(c)) {
          ss << (char)c;
          return new Token(startLoc,Token::TOKEN_TYPE_SPECIAL,ss.str());
        }

        ss << (char)c;
        // cout << "START OF TOKEN at " << loc.toString() << endl;
        while (1) {
          lastLoc = loc;
          c = get_char();
          if (c < 0)
            return new Token(startLoc,Token::TOKEN_TYPE_LITERAL,ss.str());
          if (c == '#' || isSpecial(c) || isWhite(c)) {
            // cout << "END OF TOKEN AT " << lastLoc.toString() << endl;
            // PRINT((char)c);
            unget_char(c);
            // PRINT(ss.str());
            return new Token(startLoc,Token::TOKEN_TYPE_LITERAL,ss.str());
          }
          ss << (char)c;
        }
      }

      Ref<File> file;
      Loc loc;
      int peekedChar;
    };


    struct ParserException {
      ParserException(const std::string &error,
                      const std::string &where="")
        : error(error)
      { if (where != "") addBackTrace(where); }
      void addBackTrace(const std::string &where) 
      { backTrace.push_back(where); }
      std::string toString() const {
        if (backTrace.empty()) return error;
        std::string msg = error + "\nBacktrace:\n";
        for (int i=0;i<backTrace.size();i++)
          msg += (backTrace[i]+"\n");
        return msg;
      }
      std::string error;
      std::vector<std::string> backTrace;
    };


    float parseFloat(Tokenizer &tokens)
    {
      Ref<Token> token = tokens.next();
      if (token == Token::TOKEN_EOF)
        throw new ParserException("unexpected end of file",__PRETTY_FUNCTION__);
      return atof(token->text.c_str());
    }

    vec3f parseVec3f(Tokenizer &tokens)
    {
      try {
        const float x = parseFloat(tokens);
        const float y = parseFloat(tokens);
        const float z = parseFloat(tokens);
        return vec3f(x,y,z);
      } catch (ParserException *e) {
        e->addBackTrace(__PRETTY_FUNCTION__);
        throw e;
      }
    }

    Ref<Param> parseParam(std::string &name, Tokenizer &tokens)
    {
      Ref<Token> token = tokens.peek();
      if (token->type != Token::TOKEN_TYPE_STRING)
        return NULL;

      token = tokens.next();
      char *_type = strdup(token->text.c_str());
      char *_name = strdup(token->text.c_str());
      int rc = sscanf(token->text.c_str(),"%s %s",_type,_name);
      if (rc != 2)
        throw new ParserException("could not parse object parameter's type and name "+token->loc.toString(),
                                  __PRETTY_FUNCTION__);
      string type = _type;
      name = _name;
      free(_name);
      free(_type);

      Ref<Param> ret = NULL;
      if (type == "float") {
        ret = new ParamT<float>;
      } else if (type == "color") {
        ret = new ParamT<float>;
      } else if (type == "integer") {
        ret = new ParamT<int>;
      } else if (type == "bool") {
        ret = new ParamT<bool>;
      } else if (type == "string") {
        ret = new ParamT<std::string>;
      } else {
        throw new ParserException("unknown parameter type '"+type+"' "+token->loc.toString(),
                                  __PRETTY_FUNCTION__);
      }

      if (tokens.peek()->text == "[") {
        tokens.next();
        Ref<Token> p = tokens.next();

        while (p->text != "]") {
          ret->add(p->text);
          p = tokens.next();
        }
      }
      return ret;
    }

    void parseParams(Ref<Node> node, Tokenizer &tokens)
    {
      while (1) {
        std::string name;
        Ref<Param> param = parseParam(name,tokens);
        if (!param) return;
        node->param[name] = param;
      }
    }

    // /*! parse given file, and add it to the scene we hold */
    // void Parser::parseWorld(Tokenizer &tokenizer)
    // {
    //   try {
    //     while (1) {
    //       Loc lastLoc = tokenizer.getLastLoc();
    //       Ref<Token> token = tokenizer.next();

    //       if (!token)
    //         throw new ParserException("unexpected end of file at "+lastLoc.toString()+": expected WorldEnd");
          
    //       if (token->text == "AttributeBegin") {
    //         pushAttributes();
    //         continue;
    //       }

    //       if (token->text == "AttributeEnd") {
    //         popAttributes();
    //         continue;
    //       }

    //       if (token->text == "WorldEnd")
    //         return;

    //       throw new ParserException("unexpected token '"+token->text+"' at "+token->loc.toString());
    //     }
    //   } catch (ParserException *e) {
    //     e->addBackTrace(__PRETTY_FUNCTION__);
    //     throw e;
    //   }
    // }

    /*! parse given file, and add it to the scene we hold */
    void Parser::parse(const FileName &fn)
    {
      try {
        std::stack<Tokenizer *> tokenizerStack;
        Tokenizer *tokens = new Tokenizer(fn);

        while (1) {
          Ref<Token> token = tokens->next();
          if (!token) {
            if (tokenizerStack.empty())
              break;
            tokens = tokenizerStack.top();
            tokenizerStack.pop();
            continue;
          }

          if (token->text == "Include") {
            Ref<Token> fileNameToken = tokens->next();
            FileName includedFileName = fileNameToken->text;
            if (includedFileName.str()[0] != '/') {
              includedFileName = tokens->file->name.path()+includedFileName;
            }
            cout << "... including file '" << includedFileName.str() << " ..." << endl;
            tokenizerStack.push(tokens);
            tokens = new Tokenizer(includedFileName);
            continue;
          }

          if (token->text == "Scale") {
            vec3f scale = parseVec3f(*tokens);
            cout << "Scale " << scale << endl;
            continue;
          }
          if (token->text == "Rotate") {
            vec3f axis = parseVec3f(*tokens);
            float angle = parseFloat(*tokens);
            cout << "Rotate " << axis << ":" << angle << endl;
            continue;
          }
          if (token->text == "LookAt") {
            vec3f v0 = parseVec3f(*tokens);
            vec3f v1 = parseVec3f(*tokens);
            vec3f v2 = parseVec3f(*tokens);
            cout << "LookAt " << v0 << " " << v1 << " " << v2 << endl;
            continue;
          }
          if (token->text == "Camera") {
            Ref<Camera> camera = new Camera(tokens->next()->text);
            parseParams(camera.cast<Node>(),*tokens);
            continue;
          }
          if (token->text == "LightSource") {
            Ref<LightSource> lightSource = new LightSource(tokens->next()->text);
            parseParams(lightSource.cast<Node>(),*tokens);
            continue;
          }
          if (token->text == "Film") {
            Ref<Film> film = new Film(tokens->next()->text);
            parseParams(film.cast<Node>(),*tokens);
            continue;
          }
          if (token->text == "Renderer") {
            Ref<Renderer> renderer = new Renderer(tokens->next()->text);
            parseParams(renderer.cast<Node>(),*tokens);
            continue;
          }

          if (token->text == "AttributeBegin") {
            pushAttributes();
            continue;
          }
        
          if (token->text == "AttributeEnd") {
            popAttributes();
            continue;
          }
        
          if (token->text == "WorldBegin") {
            continue;
          }
          if (token->text == "WorldEnd") {
            continue;
          }
          
          
        
          throw new ParserException("unexpected token '"+token->text+"' at "+token->loc.toString());
        }
      } catch (ParserException *e) {
        THROW_RUNTIME_ERROR("plib::pbrt parser fatal error:\n"+e->toString());
      }
    }

  }
}
